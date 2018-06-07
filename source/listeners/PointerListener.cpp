#include "listeners/PointerListener.hpp"

PointerListener::PointerListener()
{
    pointerPosition = {0.0, 0.0};
    button = {LEFT, 0};
}

void PointerListener::pointerEnter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY)
{
  std::cout << "pointer enter" << std::endl;
}

void PointerListener::pointerLeave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
{
  std::cout << "pointer leave" << std::endl;
}

void PointerListener::pointerMotion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
  pointerPosition.x = wl_fixed_to_double(x);
  pointerPosition.y = wl_fixed_to_double(y);
}

void PointerListener::pointerButton(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  this->button.button = button;
  this->button.state = state;

  std::cout << "pointer button (button " << button << ", state " << state << ")" <<
  " at (x: " << pointerPosition.x << ", y: "  << pointerPosition.y << ")" << std::endl;
}

void PointerListener::pointerAxis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
   std::cout << "pointer axis" << std::endl;
}

PointerPosition PointerListener::getPositions()
{
  return pointerPosition;
}

MouseButton PointerListener::getButton()
{
  return button;
}
