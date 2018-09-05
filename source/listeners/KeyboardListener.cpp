#include "listeners/KeyboardListener.hpp"

namespace wayland_client
{

  KeyboardListener::KeyboardListener()
    : xkbContext(xkb_context_new (XKB_CONTEXT_NO_FLAGS))
  {

  }

  void KeyboardListener::keyboardKeymap(struct wl_keyboard *, [[maybe_unused]]uint32_t format, int32_t fd, uint32_t size)
  {
    char *keymapString = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));

    xkb_keymap_unref(keymap);
    keymap = xkb_keymap_new_from_string(xkbContext, keymapString, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap (keymapString, size);
    close(fd);
    xkb_state_unref(xkbState);
    xkbState = xkb_state_new(keymap);
  //  std::cout << "KEYMAP: " << xkb_keymap_get_as_string(keymap,  XKB_KEYMAP_FORMAT_TEXT_V1) << std::endl;
  }

  void KeyboardListener::keyboardEnter(struct wl_keyboard *, [[maybe_unused]]uint32_t serial, struct wl_surface *, [[maybe_unused]]struct wl_array *keys)
  {
    std::cout << "Entering the window" << std::endl;
  }

  void KeyboardListener::keyboardLeave(struct wl_keyboard *, [[maybe_unused]]uint32_t serial, struct wl_surface *)
  {
    std::cout << "Leaving the window" << std::endl;
  }

  void KeyboardListener::keyboardKey(struct wl_keyboard *, [[maybe_unused]]uint32_t serial, [[maybe_unused]]uint32_t time, uint32_t key, uint32_t state)
  {
    //Offset to get the correct ascii code in the table. The key map String start to 9, so you have to shift the code by 8
    constexpr int offset = 8;

    if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
    {
      xkb_keysym_t keysym = xkb_state_key_get_one_sym(xkbState, key + offset);
      uint32_t utf32 = xkb_keysym_to_utf32(keysym);
      if (utf32) {
        if (utf32 >= 0x21 && utf32 <= 0x7E) {
          std::cout << "the key " << static_cast<char>(utf32) << " was pressed" << std::endl;
          if (utf32 == 'q')
            running = false;
        }
        else {
          printf ("the key U+%04X was pressed\n", utf32);
        }
      }
      else {
        char name[64];
        xkb_keysym_get_name(keysym, name, 64);
        std::cout << "the key " << name << " was pressed" << std::endl;
      }
    }
  }

  void KeyboardListener::keyboardModifiers(struct wl_keyboard *, [[maybe_unused]]uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
  {
    xkb_state_update_mask(xkbState, modsDepressed, modsLatched, modsLocked, 0, 0, group);
  }

  bool KeyboardListener::getRunning() const
  {
    return running;
  }
}
