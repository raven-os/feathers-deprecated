#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <map>
#include <string>

class WindowListener {
  public:
    WindowListener(std::map<std::string, Listener> &listeners);
    ~WindowListener() = default;

  private:
    static void shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
    static void shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    static void shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface);
};
