#include "listeners/PointerListener.hpp"

void PointerListener::pointer_enter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  std::cout << "pointer enter" << std::endl;
}

void PointerListener::pointer_leave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
{
  std::cout << "pointer leave" << std::endl;
}

void PointerListener::pointer_motion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
  std::cout << "pointer motion x: " << wl_fixed_to_double(x) << ", y: " << wl_fixed_to_double(y) << std::endl;
}

void PointerListener::pointer_button(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  std::cout << "pointer button (button " << button << ", state " << state << ")" << std::endl;
}

void PointerListener::pointer_axis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
   std::cout << "pointer axis" << std::endl;
}
