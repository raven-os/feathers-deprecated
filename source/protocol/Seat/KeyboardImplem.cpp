#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include "protocol/Seat/KeyboardImplem.hpp"
#include "utils/os-compatibility.hpp"

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
    FthKeyboard *keyboard;

    if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
      std::cerr << "Client sent get_keyboard on seat without the keyboard capability" << std::endl;
      return;
    }
    instantiateImplementation(client, version, id, wl_keyboard_interface, &keyboard_implementation, seat_client, [](wl_resource *){
      std::cout << "Keyboardcreated!" << std::endl;
    });
    keyboard = seat_client->seat->keyboard_state.keyboard;
    sendKeymap(keyboard);
    sendRepeatInfo(keyboard);
  }

  void KeyboardImplem::sendKeymap(FthKeyboard *keyboard)
  {
    wl_resource *resource;
    int keymap_fd;

    if (!keyboard)
    {
      return;
    }
    wl_resource_for_each(resource, &seat_client->keyboards)
    {
      if ((keymap_fd = os_create_anonymous_file(keyboard->keymap_size)) < 0) {
        continue;
      }
      char *map = static_cast<char *>(mmap(nullptr, keyboard->keymap_size, PROT_READ | PROT_WRITE, MAP_SHARED, keymap_fd, 0));
      if (map == MAP_FAILED) {
        close(keymap_fd);
        continue;
      }
      strcpy(map, keyboard->keymap_string);
      munmap(map, keyboard->keymap_size);

      wl_keyboard_send_keymap(resource, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keymap_fd, keyboard->keymap_size);
      std::cout << "Keyboard has send" << std::endl;

		  close(keymap_fd);
    }
  }

  void KeyboardImplem::sendRepeatInfo(FthKeyboard *keyboard)
  {
  	if (!keyboard) {
  		return;
  	}

  	struct wl_resource *resource;
  	wl_resource_for_each(resource, &seat_client->keyboards) {
  		if (!wl_resource_instance_of(resource, &wl_keyboard_interface, this) || wl_resource_get_user_data(resource) == NULL) {
  			continue;
  		}

  		if (wl_resource_get_version(resource) >=
  				WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION) {
  			wl_keyboard_send_repeat_info(resource,
  				keyboard->repeat_info.rate, keyboard->repeat_info.delay);
  		}
  	}
  }

}
