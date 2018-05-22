#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <map>
#include <variant>
#include <iostream>
#include <string>
#include "user-input/pointerListener.hpp"
#include "user-input/keyboardListener.hpp"
#include "user-input/seatListener.hpp"
#include "user-input/registryListener.hpp"
#include "user-input/windowListener.hpp"

using var_listener  = std::variant<struct wl_pointer_listener,
             struct wl_keyboard_listener,
             struct wl_seat_listener,
             struct wl_registry_listener,
             struct wl_shell_surface_listener>;


class Listeners {
  public:
    Listeners();
    ~Listeners();
    var_listener const &getListener(std::string key) const;

  private:
    std::map<std::string, var_listener> listeners;
};
