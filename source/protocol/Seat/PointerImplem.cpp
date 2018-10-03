#include "protocol/Seat/PointerImplem.hpp"

namespace protocol
{
  void PointerImplem::set_cursor(struct wl_client *client,
        struct wl_resource *resource,
        uint32_t serial,
        struct wl_resource *surface,
        int32_t hotspot_x,
        int32_t hotspot_y)
  {

  }

  void PointerImplem::release([[maybe_unused]]struct wl_client *client, struct wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }
}
