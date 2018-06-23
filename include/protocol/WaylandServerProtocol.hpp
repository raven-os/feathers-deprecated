#include <wayland-server.h>

namespace protocol
{
  class WaylandServerProtocol
  {
  private:
    struct wl_display *wlDisplay;
    struct wl_event_loop *wlEventLoop;
    struct wl_protocol_logger *wlProtocolLogger;

  public:
    WaylandServerProtocol();
    ~WaylandServerProtocol();

    int32_t AddSocket();
    int32_t AddSocket(std::string const &);
    void AddProtocolLogger(wl_protocol_logger_func_t func, void *user_data);
    void EventDispatch(int32_t timeout);

    WaylandServerProtocol(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol(WaylandServerProtocol &&) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol &&) = delete;
  };
}
