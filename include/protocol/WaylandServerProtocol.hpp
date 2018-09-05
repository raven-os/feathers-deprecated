#include <wayland-server.h>

namespace protocol
{
  class WaylandServerProtocol : private wl_listener // inheriting to avoid using offset_of which is not non-pod friendly
  {
  private:
    struct wl_display *wlDisplay;
    struct wl_event_loop *wlEventLoop;
    struct wl_protocol_logger *wlProtocolLogger;

  public:
    WaylandServerProtocol();
    ~WaylandServerProtocol();

    int32_t addSocket();
    int32_t addSocket(std::string const &);
    void addProtocolLogger(wl_protocol_logger_func_t func, void *user_data);
    void eventDispatch(int32_t timeout);
    void process(struct wl_client *data);

    WaylandServerProtocol(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol(WaylandServerProtocol &&) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol &&) = delete;
  };
}
