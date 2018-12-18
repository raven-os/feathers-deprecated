#include <iostream>
#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{

  KeyboardImplem::KeyboardImplem(wl_resource *resource) {
    version = wl_resource_get_version(resource);
    fthKeyboard = new FthKeyboard();
  }

  void KeyboardImplem::createImplem(wl_client *client, uint32_t id)
  {
    static auto keyboard_implementation(createImplementation<struct wl_keyboard_interface,
                &KeyboardImplem::release
                >());
    wl_resource *resource;
    FthKeyboard *keyboard = dynamic_cast<FthKeyboard*>(this->fthKeyboard);

    // if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
    //   std::cerr << "Client sent get_keyboard on seat without the keyboard capability" << std::endl;
    //   return;
    // }
    resource = instantiateImplementation(client, version, id, wl_keyboard_interface, &keyboard_implementation, fthKeyboard, [](wl_resource *){
      std::cout << "Keyboard unbind!" << std::endl;
    });

    wl_keyboard_send_keymap(resource, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keyboard->getXkbHandler().keymap.fd, keyboard->getXkbHandler().keymap.size - 1);

    if (wl_resource_get_version(resource) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION) {
      wl_keyboard_send_repeat_info(resource, keyboard->getRate(), keyboard->getDelay());
    }
  }

  void KeyboardImplem::release([[maybe_unused]]wl_client *client, wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }
}