#pragma once

#include <wayland-client.h>
#include "display/WaylandAdapters.hpp"
#include "listeners/KeyboardListener.hpp"
#include "listeners/PointerListener.hpp"

class SeatListener {

  KeyboardListener *keyboard_listener;
  PointerListener *pointer_listener;

  public:
    SeatListener();
    ~SeatListener() = default;

    void seat_capabilities(struct wl_seat *seat, uint32_t capabilities);
    void seat_name(struct wl_seat *seat, const char *name);

    bool getRunning() const;
};
