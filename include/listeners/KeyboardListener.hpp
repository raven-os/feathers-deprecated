#pragma once

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <sys/mman.h>
#include <unistd.h>

class KeyboardListener
{
  struct xkb_context *xkb_context;
  struct xkb_keymap *keymap{nullptr};
  struct xkb_state *xkb_state{nullptr};

  bool running = true;

  public:
    KeyboardListener();
    ~KeyboardListener() = default;

    void keyboard_keymap(struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
    void keyboard_enter(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
    void keyboard_leave(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
    void keyboard_key(struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    void keyboard_modifiers(struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);

    bool getRunning() const;
};
