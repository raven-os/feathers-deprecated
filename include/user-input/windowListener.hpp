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

class windowListener {
  public:
    windowListener(std::map<std::string, var_listener> &listeners);
    ~windowListener() = default;

  private:
    static void shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
    static void shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    static void shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface);
};
