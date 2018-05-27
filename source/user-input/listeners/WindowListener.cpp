#include "user-input/WindowHandler.hpp"
#include "user-input/WindowListener.hpp"

WindowListener::WindowListener(std::map<std::string, Listener> &listeners) {
  const struct wl_shell_surface_listener shell_surface_listener =
        {&WindowListener::shell_surface_ping,
         &WindowListener::shell_surface_configure,
         &WindowListener::shell_surface_popup_done};
  listeners.emplace("shell", shell_surface_listener);
}

void WindowListener::shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
	wl_shell_surface_pong (shell_surface, serial);
}

void WindowListener::shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
  struct window *window = static_cast<struct window *>(data);
	wl_egl_window_resize (window->egl_window, width, height, 0, 0);
}

void WindowListener::shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface) {

}
