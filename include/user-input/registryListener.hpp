#pragma once

#include "user-input/user-input.hpp"

using var_listener  = std::variant<struct wl_pointer_listener,
             struct wl_keyboard_listener,
             struct wl_seat_listener,
             struct wl_registry_listener,
             struct wl_shell_surface_listener>;

class registryListener {
  public:
    registryListener(std::map<std::string, var_listener> &listeners);
    ~registryListener() = default;

  private:
    static void registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void registry_remove_object (void *data, struct wl_registry *registry, uint32_t name);

};
