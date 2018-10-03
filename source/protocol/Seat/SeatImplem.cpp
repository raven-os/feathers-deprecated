#include <iostream>
#include "protocol/CreateImplementation.hpp"
#include "protocol/Seat/SeatImplem.hpp"
#include "protocol/Seat/PointerImplem.hpp"

namespace protocol
{
  void SeatImplem::get_pointer([[maybe_unused]] wl_client *client,
          [[maybe_unused]] wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
    static auto pointer_implementation(createImplementation<struct wl_pointer_interface,
                &PointerImplem::set_cursor,
                &PointerImplem::release
                >());
    FthSeatClient *seat_client = static_cast<FthSeatClient *>(wl_resource_get_user_data(resource));
    uint32_t version;

    if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_POINTER))
    {
      std::cerr << "Client sent get_pointer on seat without the pointer capability" << std::endl;
		  return ;
	  }

    version = wl_resource_get_version(resource);
    if (wl_resource *resource = wl_resource_create(client, &wl_pointer_interface, version, id))
    {
	   wl_resource_set_implementation(resource, &pointer_implementation, this, [](wl_resource *){
       std::cout << "Pointer created!" << std::endl;
	    });
    }
    else
      wl_client_post_no_memory(client);
  }

  void SeatImplem::get_keyboard([[maybe_unused]] wl_client *client,
          [[maybe_unused]] wl_resource *resource,
          [[maybe_unused]]uint32_t id)
  {
    static auto keyboard_implementation(createImplementation<struct wl_keyboard_interface,
                &PointerImplem::release
                >());
    FthSeatClient *seat_client = static_cast<FthSeatClient *>(wl_resource_get_user_data(resource));
    uint32_t version;

    if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
  		std::cerr << "Client sent get_keyboard on seat without the keyboard capability" << std::endl;
  		return;
  	}

	   version = wl_resource_get_version(resource);
     if (wl_resource *resource = wl_resource_create(client, &wl_keyboard_interface, version, id))
     {
 	   wl_resource_set_implementation(resource, &keyboard_implementation, this, [](wl_resource *){
        std::cout << "Keyboardcreated!" << std::endl;
 	    });
     }
     else
       wl_client_post_no_memory(client);
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
