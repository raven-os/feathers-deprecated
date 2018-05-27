#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <map>
#include <variant>
#include <iostream>
#include <string>
#include "user-input/PointerListener.hpp"
#include "user-input/KeyboardListener.hpp"
#include "user-input/SeatListener.hpp"
#include "user-input/RegistryListener.hpp"
#include "user-input/WindowListener.hpp"

using Listener  = std::variant<struct wl_pointer_listener,
             struct wl_keyboard_listener,
             struct wl_seat_listener,
             struct wl_registry_listener,
             struct wl_shell_surface_listener>;


class Listeners {
  public:
    Listeners();
    ~Listeners();
    Listener const &getListener(std::string key) const;

  private:
    std::map<std::string, Listener> listeners;
};
