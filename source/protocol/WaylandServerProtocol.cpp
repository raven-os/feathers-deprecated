#include <vector>
#include <string>
#include <cassert>
#include <string.h>
#include <errno.h>

#include "protocol/WaylandServerProtocol.hpp"
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"
#include "protocol/Surface.hpp"
#include "protocol/ShellSurface.hpp"
#include "protocol/XDGShell.hpp"
#include "protocol/ShmPool.hpp"

#include "generated/xdg-shell-unstable-v6-server-protocol.h"

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

  WaylandServerProtocol::WaylandServerProtocol(display::Renderer &renderer)
    : wl_listener(),
      wlDisplay(wl_display_create()),
      wlEventLoop(wl_display_get_event_loop(wlDisplay)),
      wlProtocolLogger(nullptr),
      windowTree(wm::WindowData{{{{0, 0}}, {{1920, 1080}}}, true, wm::Container{wm::Tilling{}}}),
      renderer(renderer)
  {
    wl_global_create(wlDisplay, &wl_compositor_interface, 1, this,
		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindCompositor>());
    // wl_global_create(wlDisplay, &wl_shell_interface, 1, this,
    // 		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindShell>());
    wl_global_create(wlDisplay, &zxdg_shell_v6_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindXDGShell>());
    wl_global_create(wlDisplay, &wl_seat_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindSeat>());
    wl_global_create(wlDisplay, &wl_shm_interface, 1, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindShm>());
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

    instantiateImplementation(client, 1, id, wl_surface_interface, &surface_implementation, new Surface(&renderer), [](wl_resource *resource)
			      {
				delete static_cast<Surface *>(wl_resource_get_user_data(resource));
			      });
  }

  void WaylandServerProtocol::createRegion([[maybe_unused]] struct wl_client *client,
					   [[maybe_unused]] struct wl_resource *,
					   [[maybe_unused]] uint32_t id)
  {
    printf("TODO: create region\n");
  }

  void WaylandServerProtocol::bindCompositor(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto compositor_implementation(createImplementation<struct wl_compositor_interface,
					  &WaylandServerProtocol::createSurface,
					  &WaylandServerProtocol::createRegion>());

    instantiateImplementation(client, version, id, wl_compositor_interface, &compositor_implementation, this, [](wl_resource *)
			      {
				printf("Destroying compositor!\n"); // todo ?
			      });
  }

  void WaylandServerProtocol::getShellSurface(struct wl_client *client,
					      struct wl_resource *,
					      uint32_t id,
					      struct wl_resource *surfaceResource)
  {
    Surface *surface(static_cast<Surface *>(wl_resource_get_user_data(surfaceResource)));

    try
      {
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

	instantiateImplementation(client, 1, id, wl_shell_surface_interface, &shell_surface_implementation, new ShellSurface(surface, &windowTree),
				  [](wl_resource *resource)
				  {
				    delete static_cast<ShellSurface *>(wl_resource_get_user_data(resource));
				  });
      }
    catch (Surface::Taken)
      {
	wl_resource_post_error(surfaceResource, WL_SHELL_ERROR_ROLE, "");
      }
  }

  void WaylandServerProtocol::bindShell(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto shell_implementation(createImplementation<struct wl_shell_interface, &WaylandServerProtocol::getShellSurface>());

    instantiateImplementation(client, version, id,  wl_shell_interface,  &shell_implementation, this, [](wl_resource *)
			      {
				printf("Destroying shell!\n"); // todo ?
			      });
  }

  void WaylandServerProtocol::bindXDGShell(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto shell_implementation(createImplementation<struct zxdg_shell_v6_interface,
				     &XDGShell::destroy,
				     &XDGShell::create_positioner,
				     &XDGShell::get_xdg_surface,
				     &XDGShell::pong>());

    instantiateImplementation(client, version, id, zxdg_shell_v6_interface,  &shell_implementation, new XDGShell(windowTree), [](wl_resource *resource)
			      {
				delete static_cast<XDGShell *>(wl_resource_get_user_data(resource));
			      });
  }

  void WaylandServerProtocol::bindSeat(struct wl_client *client, uint32_t version, uint32_t id)
  {
    printf("bindSeat called!\n");
  }

  /// wl_shm::create_pool
  void WaylandServerProtocol::createShmPool(struct wl_client *client, struct wl_resource *, uint32_t id, int fd, int size)
  {
    static auto shm_pool_implementation(createImplementation<struct wl_shm_pool_interface,
					&ShmPool::create_buffer,
					&ShmPool::destroy,
					&ShmPool::resize>());

    instantiateImplementation(client, 1, id, wl_shm_pool_interface, &shm_pool_implementation, new ShmPool(fd, size), [](wl_resource *resource)
                              {
                                delete static_cast<ShmPool *>(wl_resource_get_user_data(resource));
                              });
    
  }

  void WaylandServerProtocol::bindShm(struct wl_client *client, uint32_t version, uint32_t id)
  {
    static auto shm_implementation(createImplementation<struct wl_shm_interface, &WaylandServerProtocol::createShmPool>());

    printf("TODO: post formats\n");
    wl_resource *shm = instantiateImplementation(client, version, id,  wl_shm_interface,  &shm_implementation, this, [](wl_resource *)
														     {
														       printf("Destroying shm!\n"); // todo ?
														     });
    wl_shm_send_format(shm, WL_SHM_FORMAT_ARGB8888);
    wl_shm_send_format(shm, WL_SHM_FORMAT_XRGB8888);
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

  wm::WindowTree const &WaylandServerProtocol::getWindowTree() const noexcept
  {
    return windowTree;
  }

  struct wl_display *WaylandServerProtocol::getWaylandDisplay() const noexcept
  {
    return wlDisplay;
  }
}
