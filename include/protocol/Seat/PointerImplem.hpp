#pragma once

#include <wayland-server.h>
#include "protocol/Seat/FeatherSeat.hpp"

namespace protocol
{
  class PointerImplem
  {
    public:
      PointerImplem() = delete;

      void set_cursor(struct wl_client *client, struct wl_resource *resource, uint32_t serial, struct wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y);
      void release(struct wl_client *client, struct wl_resource *resource);
  };
}
