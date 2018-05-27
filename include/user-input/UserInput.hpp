#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>
#include <string.h>
#include <stdio.h>
#include <variant>

class Listeners;
class WindowHandler;

// typedef struct wl_pointer_listener pointerListener;
// typedef struct wl_keyboard_listener keyboardListener;
// typedef struct wl_seat_listener seatListener;
// typedef struct wl_registry_listener registryListener;
// typedef struct wl_shell_surface_listener windowListener;

using Listener  = std::variant<struct wl_pointer_listener,
             struct wl_keyboard_listener,
             struct wl_seat_listener,
             struct wl_registry_listener,
             struct wl_shell_surface_listener>;

class UserInput {
public:
  static UserInput &get();
  static void createWindow();

  WindowHandler &getWindow();

  struct wl_display *display;
  struct wl_compositor *compositor;
  struct wl_shell *shell;
  struct wl_seat *seat;
  EGLDisplay egl_display;
  struct xkb_context *xkb_context;
  struct xkb_keymap *keymap;
  struct xkb_state *xkb_state;
  bool running;
  Listeners *listeners;

private:
  UserInput();

private:
  static UserInput instance;

  WindowHandler *win;
  //std::queue<T> events;
};
