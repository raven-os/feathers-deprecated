#include <iostream>
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  SeatImplem::SeatImplem()
    : name("seat0"), capabilities(0)
  {
    // TODO
    wl_list_init(&resources);
    // seat.swc_listener.notify = &handle_swc_event;
    // wl_signal_add(&swc.event_signal, &seat.swc_listener);

    // if (!data_device_initialize(&seat.data_device)) {
    //   ERROR("Could not initialize data device\n");
    //   goto error2;
    // }

    // wl_signal_add(&seat.data_device.event_signal, &data_device_listener);

    // if (!keyboard_initialize(&seat.keyboard)) {
    //   ERROR("Could not initialize keyboard\n");
    //   goto error3;
    // }

    // wl_signal_add(&seat.keyboard.focus.event_signal, &keyboard_focus_listener);

    // if (!pointer_initialize(&seat.pointer)) {
    //   ERROR("Could not initialize pointer\n");
    //   goto error4;
    // }

    // if (!initialize_libinput(seat.name))
    //   goto error5;

    printf("Seat binded\n");
  }

  void SeatImplem::get_pointer(wl_client *client,
			       wl_resource *resource,
			       uint32_t id)
  {
    pointer = new PointerImplem(resource);
    pointer->createImplem(client, id);
    std::cout << "get pointer" << std::endl;
  }

  void SeatImplem::get_keyboard(wl_client *client,
				wl_resource *resource,
				uint32_t id)
  {
    keyboard = new KeyboardImplem(resource);
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
