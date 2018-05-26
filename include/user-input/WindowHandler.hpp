#pragma once

#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <variant>
#include "user-input/UserInput.hpp"
#include "user-input/Listeners.hpp"

struct window {
  EGLContext egl_context;
  struct wl_surface *surface;
  struct wl_shell_surface *shell_surface;
  struct wl_egl_window *egl_window;
  EGLSurface egl_surface;
};

class WindowHandler {
public:
  WindowHandler(EGLDisplay &egl_display, struct wl_shell *shell,
		struct wl_compositor *compositor);
  ~WindowHandler();

  void draw();

private:
  static constexpr int WIDTH = 256;
  static constexpr int HEIGHT = 256;
  EGLDisplay egl_display;
  struct wl_shell *shell;
  struct window window;
};
