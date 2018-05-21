keyboardListener::keyboardListener(std::map<std::string, var_listener> &listeners) {
  static struct wl_keyboard_listener keyboard_listener =
        {&keyboardListener::keyboard_keymap,
         &keyboardListener::keyboard_enter,
         &keyboardListener::keyboard_leave,
         &keyboardListener::keyboard_key,
         &keyboardListener::keyboard_modifiers};
  listeners.emplace("keyboard", keyboard_listener);
}


void keyboardListener::keyboard_keymap (void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
  char *keymap_string = static_cast<char *>(mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));

	xkb_keymap_unref(userInput::get().keymap);
	userInput::get().keymap = xkb_keymap_new_from_string(userInput::get().xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap (keymap_string, size);
	close(fd);
	xkb_state_unref(userInput::get().xkb_state);
	userInput::get().xkb_state = xkb_state_new(userInput::get().keymap);
}

void keyboardListener::keyboard_enter (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
  printf("Entering the window\n");
}

void keyboardListener::keyboard_leave (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
  printf("Leaving the window\n");
}

void keyboardListener::keyboard_key (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
	if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		xkb_keysym_t keysym = xkb_state_key_get_one_sym(userInput::get().xkb_state, key+8);
		uint32_t utf32 = xkb_keysym_to_utf32(keysym);
		if (utf32) {
			if (utf32 >= 0x21 && utf32 <= 0x7E) {
				printf ("the key %c was pressed\n", (char)utf32);
				if (utf32 == 'q') userInput::get().running = false;
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

void keyboardListener::keyboard_modifiers (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
	xkb_state_update_mask(userInput::get().xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}
