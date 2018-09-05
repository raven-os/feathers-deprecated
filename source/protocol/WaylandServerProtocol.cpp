#include <vector>
#include <string>
#include <cassert>
#include <string.h>
#include <errno.h>
#include "protocol/WaylandServerProtocol.hpp"

namespace protocol
{
  WaylandServerProtocol::WaylandServerProtocol()
    : wl_listener(),
      wlDisplay(wl_display_create()),
      wlEventLoop(wl_display_get_event_loop(wlDisplay)),
      wlProtocolLogger(nullptr)
  {
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
