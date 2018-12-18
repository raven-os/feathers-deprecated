#include <iostream>
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  SeatImplem::SeatImplem() {
    printf("Seat binded\n");
  }

  void SeatImplem::get_pointer(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    PointerImplem *pointer = new PointerImplem(resource);
    fthSeat.pointer = new FthPointer();

    pointer->createImplem(dynamic_cast<FthPointer*>(fthSeat.pointer), client, id);
  }

  void SeatImplem::get_keyboard(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    KeyboardImplem *keyboard = new KeyboardImplem(resource);
    fthSeat.keyboard = new FthKeyboard();

    keyboard->createImplem(dynamic_cast<FthKeyboard*>(fthSeat.keyboard), client, id);
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
