#include <vector>
#include <string>
#include <cassert>
#include <string.h>
#include <errno.h>
#include "protocol/WaylandServerProtocol.hpp"

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
      wlProtocolLogger(nullptr)
  {
    wl_global_create(wlDisplay, &wl_compositor_interface, wl_compositor_interface.version, this,
		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindCompositor>());
    wl_global_create(wlDisplay, &wl_surface_interface, wl_surface_interface.version, this,
		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindSurface>());
    wl_global_create(wlDisplay, &wl_shell_interface, wl_shell_interface.version, this,
    		     convertToWlGlobalBindFunc<&WaylandServerProtocol::bindShell>());
    wl_global_create(wlDisplay, &wl_seat_interface, wl_seat_interface.version, this,
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

  void WaylandServerProtocol::createSurface(struct wl_client *client, uint32_t id)
  {
    printf("TODO: create surface\n");
  }

  void WaylandServerProtocol::destroySurface(struct wl_client *client, uint32_t id)
  {
    printf("TODO: destroy surface\n");
  }

  static struct wl_compositor_interface compositor_implementation
  {
    [](struct wl_client *client,
       struct wl_resource *resource,
       uint32_t id)
      {
	static_cast<WaylandServerProtocol*>(wl_resource_get_user_data(resource))->createSurface(client, id);
      },
      [](struct wl_client *client,
	 struct wl_resource *resource,
	 uint32_t id)
	{
	  static_cast<WaylandServerProtocol*>(wl_resource_get_user_data(resource))->destroySurface(client, id);
	}
  };

  void WaylandServerProtocol::bindCompositor(struct wl_client *client, uint32_t version, uint32_t id)
  {
    if (wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, wl_compositor_interface.version, id))
      {
	wl_resource_set_implementation(resource, &compositor_implementation, this, [](wl_resource *resource){
	  }); // todo
      }
    else
      wl_client_post_no_memory(client);
  }

  void WaylandServerProtocol::bindSurface(struct wl_client *client, uint32_t version, uint32_t id)
  {
    printf("bindSurface called!\n");
  }

  void WaylandServerProtocol::bindShell(struct wl_client *client, uint32_t version, uint32_t id)
  {
    printf("bindShell called!\n");
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
}
