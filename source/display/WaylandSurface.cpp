#include "display/WaylandSurface.hpp"

namespace display
{
  WaylandSurface::WaylandSurface()
    : wlDisplay(wl_display_connect(NULL))
  {
    if (!wlDisplay)
      throw std::runtime_error("Could not connect to display");
    if (!(wlRegistry = wl_display_get_registry(wlDisplay)))
      throw std::runtime_error("Could not get registry");
    if (int err = addListener(wlRegistry, *this))
      {
	std::cerr << "error: " << err << std::endl;
	throw std::runtime_error("Recieved error");
      }
    wl_display_roundtrip(wlDisplay);
    if (!wlCompositor) {
      throw std::runtime_error("Could not find compositor");
    }
    if (!wlShell) {
      throw std::runtime_error("Could not find shell");
    }
    if (!wlSeat) {
      throw std::runtime_error("Could not find seat");
    }
    wlShellSurface = wl_shell_get_shell_surface(wlShell, wlSurface);
    if (!wlShellSurface) {
      throw std::runtime_error("Could not get shell surface");
    }
    addListener(wlShellSurface, *this);
    wl_shell_surface_set_toplevel(wlShellSurface);
    wl_display_roundtrip(wlDisplay);
  }

  void WaylandSurface::shell_surface_ping (struct wl_shell_surface *shell_surface, uint32_t serial)
  {
    wl_shell_surface_pong (shell_surface, serial);
  }

  void WaylandSurface::shell_surface_configure(struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
  {
    std::cout << "shell_surface_configure" << std::endl;
  }

  void WaylandSurface::shell_surface_popup_done(struct wl_shell_surface *shell_surface)
  {
  }

  void WaylandSurface::registry_add_object(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
  {
    if (!strcmp(interface,"wl_compositor"))
      {
	wlCompositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
	wlSurface = wl_compositor_create_surface(wlCompositor);
	if (!wlSurface) {
	  throw std::runtime_error("Could not create surface");
	}
      }
    else if (!strcmp(interface,"wl_shell"))
      {
	wlShell = static_cast<wl_shell *>(wl_registry_bind(registry, name, &wl_shell_interface, 1));
      }
    else if (!strcmp(interface,"wl_seat"))
      {
	wlSeat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
	// TODO: convert to new style
	// wl_seat_add_listener(UserInput::get().seat,
	// 		     &std::get<struct wl_seat_listener>(
	// 							UserInput::get().listeners->getListener("seat")),
	// 		     NULL);
      }
  }

  void WaylandSurface::registry_remove_object(struct wl_registry *registry, uint32_t name) {
  }

  void WaylandSurface::dispatch() {
    wl_display_dispatch_pending(wlDisplay);
  }

}
