#include "protocol/WaylandServerProtocol.hpp"

protocol::WaylandServerProtocol::WaylandServerProtocol()
{
  wlDisplay = wl_display_create();
  wlEventLoop = wl_display_get_event_loop(wlDisplay);
}

protocol::WaylandServerProtocol::~WaylandServerProtocol()
{
  if (wlProtocolLogger)
    wl_protocol_logger_destroy(wlProtocolLogger);
  wl_display_destroy(wlDisplay);
}

int32_t protocol::WaylandServerProtocol::AddSocket(std::string const &name)
{
  return wl_display_add_socket(wlDisplay, (name.compare("")) ? name.c_str() : nullptr);
}

void protocol::WaylandServerProtocol::AddProtocolLogger(wl_protocol_logger_func_t func,
							void *userData)
{
  wlProtocolLogger = wl_display_add_protocol_logger(wlDisplay, func, userData);
  if (errno)
    {
      fprintf(stderr,
	      "WaylandServerProtocol error: addProtocolLogger: %s\n",
	      strerror(errno));
      wlProtocolLogger = nullptr;
    }
}
