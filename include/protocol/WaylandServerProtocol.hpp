#include <wayland-server.h>
#include <vector>
#include <string>
#include <string.h>
#include <errno.h>

namespace protocol
{
  class WaylandServerProtocol
  {
  private:
    struct wl_display *wlDisplay;
    struct wl_event_loop *wlEventLoop;
    struct wl_protocol_logger *wlProtocolLogger = nullptr;

  public:
    WaylandServerProtocol();
    ~WaylandServerProtocol();

    int32_t AddSocket(std::string const &);
    void AddProtocolLogger(wl_protocol_logger_func_t func, void *user_data);

    WaylandServerProtocol(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol(WaylandServerProtocol &&) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol &&) = delete;
  };
}
