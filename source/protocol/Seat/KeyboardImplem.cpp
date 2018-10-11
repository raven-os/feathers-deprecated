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
    instantiateImplementation(client, version, id, wl_keyboard_interface, &keyboard_implementation, seat_client, [](wl_resource *){
      std::cout << "Keyboardcreated!" << std::endl;
    });
  }
}
