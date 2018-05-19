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
#include "user-input/window.hpp"

class userInput {
public:
  static userInput get();
  static void createWindow();

  WindowHandler &getWindow();
/*  template<typename T>
  void set(T&, T);*/
  void setDisplay(struct wl_display *disp) { instance.display = disp; }
  void setCompositor(struct wl_compositor *comp) { instance.compositor = comp; }
  void setShell(struct wl_shell * sh) { instance.shell = sh; }
  void setSeat(struct wl_seat *wl_seat) { instance.seat = wl_seat; }
  void setEglDisplay(EGLDisplay egl_disp) { instance.egl_display = egl_disp; }
  void setContext(struct xkb_context *ctx) { instance.xkb_context = ctx; }
  void setKeyMap(struct xkb_keymap * keyMap) { instance.keymap = keyMap; }
  void setState(struct xkb_state * st) { instance.xkb_state = st; }
  void setRun(bool run) { instance.running = run; }

  struct wl_display *display;
  struct wl_compositor *compositor;
  struct wl_shell *shell;
  struct wl_seat *seat;
  EGLDisplay egl_display;
  struct xkb_context *xkb_context;
  struct xkb_keymap *keymap;
  struct xkb_state *xkb_state;
  bool running;

private:
  userInput();

private:
  static userInput instance;

  WindowHandler *win;
  //std::queue<T> events;
};
