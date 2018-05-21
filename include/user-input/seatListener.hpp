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

class seatListener {
  public:
    seatListener(std::map<std::string, var_listener> &listeners);
    ~seatListener() = default;

  private:
    static void seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities);
};
