#include "user-input/pointerListener.hpp"

pointerListener::pointerListener(std::map<std::string, var_listener> &listeners) {
  struct wl_pointer_listener pointer_listener =
        {&pointerListener::pointer_enter,
         &pointerListener::pointer_leave,
         &pointerListener::pointer_motion,
         &pointerListener::pointer_button,
         &pointerListener::pointer_axis};
  listeners.emplace("pointer", pointer_listener);
}


void pointerListener::pointer_enter (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	printf ("pointer enter\n");
}

void pointerListener::pointer_leave (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {
	printf ("pointer leave\n");
}

void pointerListener::pointer_motion (void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
	printf ("pointer motion %f %f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
}

void pointerListener::pointer_button (void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
	printf ("pointer button (button %d, state %d)\n", button, state);
}

void pointerListener::pointer_axis (void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
	printf ("pointer axis\n");
}
