#include "user-input/PointerListener.hpp"

PointerListener::PointerListener(std::map<std::string, Listener> &listeners) {
  struct wl_pointer_listener pointer_listener =
        {&PointerListener::pointer_enter,
         &PointerListener::pointer_leave,
         &PointerListener::pointer_motion,
         &PointerListener::pointer_button,
         &PointerListener::pointer_axis};
  listeners.emplace("pointer", pointer_listener);
}


void PointerListener::pointer_enter (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	printf ("pointer enter\n");
}

void PointerListener::pointer_leave (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {
	printf ("pointer leave\n");
}

void PointerListener::pointer_motion (void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
	printf ("pointer motion %f %f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
}

void PointerListener::pointer_button (void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
	printf ("pointer button (button %d, state %d)\n", button, state);
}

void PointerListener::pointer_axis (void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
	printf ("pointer axis\n");
}
