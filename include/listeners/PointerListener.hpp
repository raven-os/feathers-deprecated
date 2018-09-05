#pragma once

#include <wayland-client.h>
#include <utility>
#include <iostream>

struct PointerPosition {
  double x;
  double y;
};

struct MouseButton {
  uint32_t button;
  uint32_t state;
};

namespace wayland_client
{
  class PointerListener
  {
    constexpr static uint32_t LEFT = 272;
    constexpr static uint32_t RIGHT = 273;

    PointerPosition pointerPosition;
    MouseButton button;

  public:
      PointerListener();
      ~PointerListener() = default;

      void pointerEnter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY);
      void pointerLeave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
      void pointerMotion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
      void pointerButton(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
      void pointerAxis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

      PointerPosition getPositions();
      MouseButton getButton();
  };
}
