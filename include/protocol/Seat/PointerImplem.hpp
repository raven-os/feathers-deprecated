#pragma once

#include <wayland-server.h>
#include "protocol/Seat/FeatherSeat.hpp"

namespace protocol
{
  class PointerImplem
  {
    public:
      PointerImplem() = delete;

      void set_cursor( wl_client *client,  wl_resource *resource, uint32_t serial,  wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y);
      void release( wl_client *client,  wl_resource *resource);
  };
}
