#include "listeners/KeyboardListener.hpp"

KeyboardListener::KeyboardListener()
  : xkb_context(xkb_context_new (XKB_CONTEXT_NO_FLAGS)) {

}

void KeyboardListener::keyboard_keymap(struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size)
{
  char *keymap_string = static_cast<char *>(mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));

  xkb_keymap_unref(keymap);
  keymap = xkb_keymap_new_from_string(xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
  munmap (keymap_string, size);
  close(fd);
  xkb_state_unref(xkb_state);
  xkb_state = xkb_state_new(keymap);
}

void KeyboardListener::keyboard_enter(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
{
  printf("Entering the window\n");
}

void KeyboardListener::keyboard_leave(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
{
  printf("Leaving the window\n");
}

void KeyboardListener::keyboard_key (struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
  if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    xkb_keysym_t keysym = xkb_state_key_get_one_sym(xkb_state, key + 8);
    uint32_t utf32 = xkb_keysym_to_utf32(keysym);
    if (utf32) {
      if (utf32 >= 0x21 && utf32 <= 0x7E) {
        printf ("the key %c was pressed\n", (char)utf32);
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
      printf ("the key %s was pressed\n", name);
    }
  }
}

void KeyboardListener::keyboard_modifiers (struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
  xkb_state_update_mask(xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

bool KeyboardListener::getRunning() const {
  return running;
}
