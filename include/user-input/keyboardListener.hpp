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


class keyboardListener {
  public:
    keyboardListener(std::map<std::string, var_listener> &listeners);
    keyboardListener() = default;

  private:
    static void keyboard_keymap (void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
    static void keyboard_enter (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
    static void keyboard_leave (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
    static void keyboard_key (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    static void keyboard_modifiers (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
};
