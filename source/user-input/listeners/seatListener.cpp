#include "user-input/seatListener.hpp"

seatListener::seatListener(std::map<std::string, var_listener> &listeners) {
  static struct wl_seat_listener seat_listener = {&seatListener::seat_capabilities};
  listeners.empalce("seat", seat_listener);
}

void seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities) {
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, NULL);
	}
	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
	}
}
