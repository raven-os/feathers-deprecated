#include <wayland-server.h>
#include "display/WindowTree.hpp"

namespace protocol
{
  class WaylandServerProtocol : private wl_listener // inheriting to avoid using offset_of which is not non-pod friendly
  {
  private:
    struct wl_display *wlDisplay;
    struct wl_event_loop *wlEventLoop;
    struct wl_protocol_logger *wlProtocolLogger;
    display::WindowTree windowTree;

  public:
    WaylandServerProtocol();
    WaylandServerProtocol(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol(WaylandServerProtocol &&) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol const &) = delete;
    WaylandServerProtocol operator=(WaylandServerProtocol &&) = delete;
    ~WaylandServerProtocol();

    int32_t addSocket();
    int32_t addSocket(std::string const &);
    void createSurface(struct wl_client *client, struct wl_resource *, uint32_t id);
    void createRegion(struct wl_client *client, struct wl_resource *, uint32_t id);

    void getShellSurface(struct wl_client *client,
			 struct wl_resource *,
			 uint32_t id,
			 struct wl_resource *surface);

    void bindCompositor(struct wl_client *client, uint32_t version, uint32_t id);
    void bindSeat(struct wl_client *client, uint32_t version, uint32_t id);
    void bindSurface(struct wl_client *client, uint32_t version, uint32_t id);
    void bindShell(struct wl_client *client, uint32_t version, uint32_t id);
    void addProtocolLogger(wl_protocol_logger_func_t func, void *user_data);
    void eventDispatch(int32_t timeout);
    void process(struct wl_client *data);

    display::WindowTree const &getWindowTree() const noexcept;
  };

}
