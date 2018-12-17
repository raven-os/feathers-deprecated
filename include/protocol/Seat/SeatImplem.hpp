#pragma once

#include <wayland-server.h>
#include "FeatherSeat.hpp"

namespace protocol
{

  class SeatImplem
  {
  public:
    SeatImplem();

    void get_pointer (wl_client *client,  wl_resource *resource, uint32_t id);
    void get_keyboard (wl_client *client,  wl_resource *resource, uint32_t id);
    void get_touch (wl_client *client,  wl_resource *resource, uint32_t id);
    void release (wl_client *client,  wl_resource *resource);

  private:
    FthSeat fthSeat;
  };
}
