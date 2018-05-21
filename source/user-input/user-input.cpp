// g++ -o user-input user-input.cpp window.cpp -lwayland-client -lwayland-egl -lEGL -lGL -lxkbcommon -I../../include/

#include "user-input/user-input.hpp"

userInput userInput::instance = userInput();

userInput::userInput() {
	userInput::instance.compositor = NULL;
	userInput::instance.shell = NULL;
	userInput::instance.seat = NULL;
	userInput::instance.keymap = NULL;
	userInput::instance.xkb_state = NULL;
	userInput::instance.running = true;
	userInput::instance.display = wl_display_connect(NULL);
	listeners = new Listeners();
}

userInput &userInput::get() {
	return instance;
}

WindowHandler &userInput::getWindow() {
	return *win;
}

void userInput::createWindow() {
	userInput::instance.win = new WindowHandler(userInput::instance.egl_display,
																							userInput::instance.shell,
																							userInput::instance.compositor);
}

// listeners
static void pointer_enter (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	printf ("pointer enter\n");
}
static void pointer_leave (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {
	printf ("pointer leave\n");
}
static void pointer_motion (void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
	printf ("pointer motion %f %f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
}
static void pointer_button (void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
	printf ("pointer button (button %d, state %d)\n", button, state);
}
static void pointer_axis (void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
	printf ("pointer axis\n");
}
static struct wl_pointer_listener pointer_listener = {&pointer_enter, &pointer_leave, &pointer_motion, &pointer_button, &pointer_axis};




static void keyboard_keymap (void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
  char *keymap_string = static_cast<char *>(mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));

	xkb_keymap_unref(userInput::get().keymap);
	userInput::get().keymap = xkb_keymap_new_from_string(userInput::get().xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap (keymap_string, size);
	close(fd);
	xkb_state_unref(userInput::get().xkb_state);
	userInput::get().xkb_state = xkb_state_new(userInput::get().keymap);
}
static void keyboard_enter (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
  printf("Entering the window\n");
}
static void keyboard_leave (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
  printf("Leaving the window\n");
}
static void keyboard_key (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
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
static void keyboard_modifiers (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
	xkb_state_update_mask(userInput::get().xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}
static struct wl_keyboard_listener keyboard_listener = {&keyboard_keymap, &keyboard_enter, &keyboard_leave, &keyboard_key, &keyboard_modifiers};




static void seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities) {
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, NULL);
	}
	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
	}
}
static struct wl_seat_listener seat_listener = {&seat_capabilities};

static void registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	if (!strcmp(interface,"wl_compositor")) {
		userInput::get().compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
	}
	else if (!strcmp(interface,"wl_shell")) {
		userInput::get().shell = static_cast<wl_shell *>(wl_registry_bind (registry, name, &wl_shell_interface, 1));
	}
	else if (!strcmp(interface,"wl_seat")) {
		userInput::get().seat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
		wl_seat_add_listener(userInput::get().seat, &seat_listener, NULL);
	}
}
static void registry_remove_object (void *data, struct wl_registry *registry, uint32_t name) {

}
static struct wl_registry_listener registry_listener = {&registry_add_object, &registry_remove_object};

int main () {
	struct wl_registry *registry = wl_display_get_registry(userInput::get().display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip (userInput::get().display);

	userInput::get().egl_display = eglGetDisplay(userInput::get().display);
	eglInitialize(userInput::get().egl_display, NULL, NULL);

	userInput::get().xkb_context = xkb_context_new (XKB_CONTEXT_NO_FLAGS);

	userInput::createWindow();

	while (userInput::get().running) {
		wl_display_dispatch_pending(userInput::get().display);
		userInput::get().getWindow().draw();
	}

//	delete win;
	eglTerminate(userInput::get().egl_display);
	wl_display_disconnect(userInput::get().display);
	return 0;
}
