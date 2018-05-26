#pragma once

#include <map>
#include "user-input/UserInput.hpp"

class PointerListener {
  public:
    PointerListener(std::map<std::string, varListener> &listeners);
    ~PointerListener() = default;

  private:
    static void pointer_enter (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
    static void pointer_leave (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
    static void pointer_motion (void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    static void pointer_button (void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    static void pointer_axis (void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

};
