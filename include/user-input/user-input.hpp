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
#include "user-input/listeners.hpp"
#include "user-input/window.hpp"

//class Listeners;

class userInput {
public:
  static userInput &get();
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
  userInput();

private:
  static userInput instance;

  WindowHandler *win;
  //std::queue<T> events;
};
