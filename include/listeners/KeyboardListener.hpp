#pragma once

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

namespace wayland_client
{
  class KeyboardListener
  {
    struct xkb_context *xkbContext;
    struct xkb_keymap *keymap{nullptr};
    struct xkb_state *xkbState{nullptr};

    bool running = true;

    public:
      KeyboardListener();
      ~KeyboardListener() = default;

      void keyboardKeymap(struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
      void keyboardEnter(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
      void keyboardLeave(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
      void keyboardKey(struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
      void keyboardModifiers(struct wl_keyboard *keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);

      bool getRunning() const;
  };
}
