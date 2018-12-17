#include <iostream>
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  SeatImplem::SeatImplem() {
    printf("SeatImplem called\n");
  }

  void SeatImplem::get_pointer(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    Implem *pointer = new PointerImplem(resource);

    pointer->createImplem(client, id);
  }

  void SeatImplem::get_keyboard(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    Implem *keyboard = new KeyboardImplem(resource);

    keyboard->createImplem(client, id);
  }

  void SeatImplem::get_touch([[maybe_unused]] wl_client *client,
          [[maybe_unused]] wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
  }

  void SeatImplem::release([[maybe_unused]] wl_client *client,
          [[maybe_unused]] wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }
}
