#include "listeners/SeatListener.hpp"

SeatListener::SeatListener() {
  keyboard_listener = new KeyboardListener();
  pointer_listener = new PointerListener();
}

void SeatListener::seat_capabilities(struct wl_seat *seat, uint32_t capabilities) {
  if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
    struct wl_pointer *pointer = wl_seat_get_pointer(seat);
    addListener(pointer, *pointer_listener);
  }
  if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
    struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
    addListener(keyboard, *keyboard_listener);
  }
}

void SeatListener::seat_name(struct wl_seat *seat, const char *name)
{

}

bool SeatListener::getRunning() const {
  return keyboard_listener->getRunning();
}
