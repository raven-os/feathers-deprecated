#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  KeyboardImplem::KeyboardImplem(wl_resource *resource) : Implem(resource)
  {
  }

  void KeyboardImplem::createImplem(wl_client *client, uint32_t id)
  {
    static auto keyboard_implementation(createImplementation<struct wl_keyboard_interface,
                &KeyboardImplem::release
                >());

    if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
      std::cerr << "Client sent get_keyboard on seat without the keyboard capability" << std::endl;
      return;
    }
   if (wl_resource *resource = wl_resource_create(client, &wl_keyboard_interface, version, id))
     {
     wl_resource_set_implementation(resource, &keyboard_implementation, seat_client, [](wl_resource *){
        std::cout << "Keyboardcreated!" << std::endl;
      });
     }
     else
       wl_client_post_no_memory(client);
  }
}
