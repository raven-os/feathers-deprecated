#include <vector>
#include <string>
#include <cassert>
#include <string.h>
#include <errno.h>

#include "protocol/WaylandServerProtocol.hpp"
#include "protocol/CreateImplementation.hpp"
#include "protocol/Surface.hpp"
#include "protocol/ShellSurface.hpp"

namespace protocol
{
  template<void (WaylandServerProtocol::*member_ptr)(struct wl_client *client, uint32_t version, uint32_t id)>
  constexpr static auto convertToWlGlobalBindFunc()
  {
    return [](struct wl_client *client, void *data, uint32_t version, uint32_t id)
      {
	(static_cast<WaylandServerProtocol*>(data)->*member_ptr)(client, version, id);
      };
  }

  WaylandServerProtocol::WaylandServerProtocol()
    : wl_listener(),
      wlDisplay(wl_display_create()),
      wlEventLoop(wl_display_get_event_loop(wlDisplay)),
      wlProtocolLogger(nullptr),
      windowTree(display::WindowData{{{{0, 0}}, {{1920, 1080}}}, true})
  {
    wl_global_create(wlDisplay, &wl_compositor_interface, 1, this,
		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindCompositor>());
    wl_global_create(wlDisplay, &wl_shell_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindShell>());
    wl_global_create(wlDisplay, &zwp_linux_dmabuf_v1_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindDmabuf>());
    wl_global_create(wlDisplay, &wl_seat_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindSeat>());
    notify = [](auto *that, void *data) {
      static_cast<WaylandServerProtocol *>(that)->process(static_cast<struct wl_client *>(data));
    };
    wl_display_add_client_created_listener(wlDisplay, this);
  }

  WaylandServerProtocol::~WaylandServerProtocol()
  {
    if (wlProtocolLogger)
      wl_protocol_logger_destroy(wlProtocolLogger);
    wl_list_remove(&link);
    wl_display_destroy(wlDisplay);
  }

  int32_t WaylandServerProtocol::addSocket()
  {
    return wl_display_add_socket(wlDisplay, nullptr);
  }

  int32_t WaylandServerProtocol::addSocket(std::string const &name)
  {
    return wl_display_add_socket(wlDisplay, name.c_str());
  }

  void WaylandServerProtocol::createSurface(struct wl_client *client, struct wl_resource *, uint32_t id)
  {
    static auto surface_implementation(createImplementation<struct wl_surface_interface,
				       &Surface::destroy,
				       &Surface::attach,
				       &Surface::damage,
				       &Surface::frame,
				       &Surface::set_opaque_region,
				       &Surface::set_input_region,
				       &Surface::commit,
				       &Surface::set_buffer_transform,
				       &Surface::set_buffer_scale,
				       &Surface::damage_buffer>());

    if (wl_resource *resource = wl_resource_create(client, &wl_surface_interface, 1, id))
      {
    	wl_resource_set_implementation(resource, &surface_implementation, new Surface(), [](wl_resource *resource){
	    delete static_cast<Surface *>(wl_resource_get_user_data(resource));
    	  });
      }
    else
      wl_client_post_no_memory(client);
 }

  void WaylandServerProtocol::createRegion([[maybe_unused]] struct wl_client *client,
					   [[maybe_unused]] struct wl_resource *,
					   [[maybe_unused]] uint32_t id)
  {
    printf("TODO: create region\n");
  }

  void WaylandServerProtocol::bindCompositor(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto compositor_implementation(createImplementation<struct wl_compositor_interface, &WaylandServerProtocol::createSurface, &WaylandServerProtocol::createRegion>());

    if (wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, version, id))
      {
	wl_resource_set_implementation(resource, &compositor_implementation, this, [](wl_resource *){
	    printf("Destroying compositor!\n"); // todo ?
	  });
      }
    else
      wl_client_post_no_memory(client);
  }

  void WaylandServerProtocol::getShellSurface(struct wl_client *client,
					      struct wl_resource *,
					      uint32_t id,
					      struct wl_resource *surfaceResource)
  {
    Surface *surface(static_cast<Surface *>(wl_resource_get_user_data(surfaceResource)));

    try
      {
	surface->setTaken();

	static auto shell_surface_implementation(createImplementation<struct wl_shell_surface_interface,
						 &ShellSurface::pong,
						 &ShellSurface::move,
						 &ShellSurface::resize,
						 &ShellSurface::set_toplevel,
						 &ShellSurface::set_transient,
						 &ShellSurface::set_fullscreen,
						 &ShellSurface::set_popup,
						 &ShellSurface::set_maximized,
						 &ShellSurface::set_title,
						 &ShellSurface::set_class
						 >());

	if (wl_resource *resource = wl_resource_create(client, &wl_shell_surface_interface, 1, id))
	  {
	    wl_resource_set_implementation(resource, &shell_surface_implementation, new ShellSurface(surface), [](wl_resource *resource){
		delete static_cast<ShellSurface *>(wl_resource_get_user_data(resource));
	      });
	    wl_shell_surface_send_configure(resource, 10 /* bot right */, 100, 100);
	  }
	else
	  wl_client_post_no_memory(client);
      }
    catch (Surface::Taken)
      {
	printf("TODO: handle wayland error\n");
      }
  }

  void WaylandServerProtocol::bindShell(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto shell_implementation(createImplementation<struct wl_shell_interface, &WaylandServerProtocol::getShellSurface>());

    if (wl_resource *resource = wl_resource_create(client, &wl_shell_interface, version, id))
      {
	wl_resource_set_implementation(resource, &shell_implementation, this, [](wl_resource *){
	    printf("Destroying shell!\n"); // todo ?
	  });
      }
    else
      wl_client_post_no_memory(client);
  }

  void WaylandServerProtocol::destroyDmabuf([[maybe_unused]] struct wl_client *client,
					    [[maybe_unused]] struct wl_resource *resource)
  {
  }

  void WaylandServerProtocol::createParams([[maybe_unused]] struct wl_client *client,
					   [[maybe_unused]] struct wl_resource *resource,
					   [[maybe_unused]] uint32_t paramId)
  {
    // static struct parmsImplementation(createImplementation<struct zwp_linux_buffer_params_v1_interface>())
  }

  void WaylandServerProtocol::bindDmabuf(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto dmabuf_implementation(createImplementation<struct zwp_linux_dmabuf_v1_interface,
				      &WaylandServerProtocol::destroyDmabuf,
				      &WaylandServerProtocol::createParams>());

    if (wl_resource *resource = wl_resource_create(client, &zwp_linux_dmabuf_v1_interface, version, id))
      {
	wl_resource_set_implementation(resource, &dmabuf_implementation, this, [](wl_resource *){
	    printf("Destroying dmabuf!\n"); // todo ?
	  });
      }
    else
      wl_client_post_no_memory(client);
  }

  void WaylandServerProtocol::bindSeat(struct wl_client *client, uint32_t version, uint32_t id)
  {
    printf("bindSeat called!\n");
  }

  void WaylandServerProtocol::addProtocolLogger(wl_protocol_logger_func_t func,
						void *userData)
  {
    wlProtocolLogger = wl_display_add_protocol_logger(wlDisplay, func, userData);
    if (errno)
      {
	fprintf(stderr,
		"WaylandServerProtocol error: AddProtocolLogger: %s\n",
		strerror(errno));
	wlProtocolLogger = nullptr;
      }
  }

  void WaylandServerProtocol::eventDispatch(int32_t timeout)
  {
    wl_display_flush_clients(wlDisplay);
    if (wl_event_loop_dispatch(wlEventLoop, timeout) == -1)
      fprintf(stderr, "Error while dispaching events\n");
  }

  void WaylandServerProtocol::process(struct wl_client *data)
  {
    pid_t pid;
    uid_t uid;
    gid_t gid;
    wl_client_get_credentials(data, &pid, &uid, &gid);
    printf("Client with pid %d, uid %d, and gid %d connected\n", pid, uid, gid);
  }

  display::WindowTree const &WaylandServerProtocol::getWindowTree() const noexcept
  {
    return windowTree;
  }
}
