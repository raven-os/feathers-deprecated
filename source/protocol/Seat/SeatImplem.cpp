#include <iostream>
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  SeatImplem::SeatImplem() {
    fthSeat.keyboard = new FthKeyboard();
    printf("SeatImplem called\n");
  }

  void SeatImplem::get_pointer(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    PointerImplem *pointer = new PointerImplem(resource);

    pointer->createImplem(fthSeat.pointer, client, id);
  }

  void SeatImplem::get_keyboard(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    KeyboardImplem *keyboard = new KeyboardImplem(resource);

    keyboard->createImplem(fthSeat.keyboard, client, id);
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
