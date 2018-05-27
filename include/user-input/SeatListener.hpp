#pragma once

#include <map>
#include "user-input/UserInput.hpp"
#include "user-input/Listeners.hpp"

class SeatListener {
  public:
    SeatListener(std::map<std::string, Listener> &listeners);
    ~SeatListener() = default;

  private:
    static void seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities);
};
