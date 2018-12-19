#include <iostream>
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  SeatImplem::SeatImplem() {
  }

  void SeatImplem::createImplem(wl_client *client, uint32_t version, uint32_t id)
  {
    static auto seat_implementation(createImplementation<struct wl_seat_interface,
                &SeatImplem::get_pointer,
                &SeatImplem::get_keyboard,
                &SeatImplem::get_touch,
                &SeatImplem::release
                >());
    wl_resource *resource;

   resource = instantiateImplementation(client, version, id, wl_seat_interface, &seat_implementation, this, [](wl_resource *){
     printf("Seat unbind!\n");
   });
   if (version >= WL_SEAT_NAME_SINCE_VERSION) {
		wl_seat_send_name(resource, name);
	 }
	 wl_seat_send_capabilities(resource, capabilities);
   printf("Seat binded\n");
  }

  void SeatImplem::get_pointer(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    pointer = new PointerImplem(resource);
    pointer->createImplem(client, id);
  }

  void SeatImplem::get_keyboard(wl_client *client,
          wl_resource *resource,
          uint32_t id)
  {
    printf("Keyboard bind 1!\n");
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
