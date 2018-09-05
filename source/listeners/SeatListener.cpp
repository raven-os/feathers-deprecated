#include "listeners/SeatListener.hpp"

SeatListener::SeatListener()
{
  keyboardListener = new KeyboardListener();
  pointerListener = new PointerListener();
}

void SeatListener::seatCapabilities(struct wl_seat *seat, uint32_t capabilities)
{
  if (capabilities & WL_SEAT_CAPABILITY_POINTER)
  {
    struct wl_pointer *pointer = wl_seat_get_pointer(seat);
    addListener(pointer, *pointerListener);
  }
  if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD)
  {
    struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
    addListener(keyboard, *keyboardListener);
  }
}

void SeatListener::seatName(struct wl_seat *, [[maybe_unused]]const char *name)
{

}

bool SeatListener::getRunning() const
{
  return keyboardListener->getRunning();
}
