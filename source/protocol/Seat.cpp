#include "protocol/Seat.hpp"

namespace protocol
{
  void Seat::get_pointer([[maybe_unused]]struct wl_client *client,
          [[maybe_unused]]struct wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
  }

  void Seat::get_keyboard([[maybe_unused]]struct wl_client *client,
          [[maybe_unused]]struct wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
  }

  void Seat::get_touch([[maybe_unused]]struct wl_client *client,
          [[maybe_unused]]struct wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
  }

  void Seat::release([[maybe_unused]]struct wl_client *client,
          [[maybe_unused]]struct wl_resource *resource)
  {
  }
}
