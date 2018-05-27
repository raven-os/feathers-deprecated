#include "user-input/SeatListener.hpp"

SeatListener::SeatListener(std::map<std::string, Listener> &listeners) {
  static struct wl_seat_listener seat_listener = {&SeatListener::seat_capabilities};
  listeners.emplace("seat", seat_listener);
}

void SeatListener::seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities) {
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer,
					&std::get<struct wl_pointer_listener>(
					  UserInput::get().listeners->getListener("pointer")),
					NULL);
	}
	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(keyboard,
					 &std::get<struct wl_keyboard_listener>(
					   UserInput::get().listeners->getListener("keyboard")),
					 NULL);
	}
}
