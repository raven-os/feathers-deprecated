#pragma once

#include <wayland-client.h>
#include "display/WaylandAdapters.hpp"
#include "listeners/KeyboardListener.hpp"
#include "listeners/PointerListener.hpp"

namespace wayland_client
{
  class SeatListener {

    KeyboardListener *keyboardListener;
    PointerListener *pointerListener;

    public:
      SeatListener();
      ~SeatListener() = default;

      void seatCapabilities(struct wl_seat *seat, uint32_t capabilities);
      void seatName(struct wl_seat *seat, const char *name);

      bool getRunning() const;
  };
}
