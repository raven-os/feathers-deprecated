#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <map>
#include <variant>
#include <string>

using var_listener  = std::variant<struct wl_pointer_listener,
             struct wl_keyboard_listener,
             struct wl_seat_listener,
             struct wl_registry_listener,
             struct wl_shell_surface_listener>;

class pointerListener {
  public:
    pointerListener(std::map<std::string, var_listener> &listeners);
    ~pointerListener() = default;

  private:
    static void pointer_enter (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
    static void pointer_leave (void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
    static void pointer_motion (void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    static void pointer_button (void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    static void pointer_axis (void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

};
