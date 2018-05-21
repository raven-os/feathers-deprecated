#include "user-input/window.hpp"
#include "user-input/windowListener.hpp"

windowListener::windowListener(std::map<std::string, var_listener> &listeners) {
  const struct wl_shell_surface_listener shell_surface_listener =
        {&windowListener::shell_surface_ping,
         &windowListener::shell_surface_configure,
         &windowListener::shell_surface_popup_done};
  listeners.emplace("shell", shell_surface_listener);
}

void windowListener::shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
	wl_shell_surface_pong (shell_surface, serial);
}

void windowListener::shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
  struct window *window = static_cast<struct window *>(data);
	wl_egl_window_resize (window->egl_window, width, height, 0, 0);
}

void windowListener::shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface) {

}
