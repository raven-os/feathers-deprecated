#include "display/WaylandSurface.hpp"

namespace display
{
  WaylandSurface::WaylandSurface()
    : wlDisplay(wl_display_connect(nullptr))
  {
    if (!wlDisplay)
      throw std::runtime_error("Could not connect to display");
    if (!(wlRegistry = wl_display_get_registry(wlDisplay)))
      throw std::runtime_error("Could not get registry");
    if (int err = addListener(wlRegistry, *this))
      {
	std::cerr << "error: " << err << std::endl;
	throw std::runtime_error("Received error");
      }
    seatListener = new SeatListener();
    wl_display_roundtrip(wlDisplay);
    if (!wlCompositor)
    {
      throw std::runtime_error("Could not find compositor");
    }
    if (!wlShell)
    {
      throw std::runtime_error("Could not find shell");
    }
    if (!wlSeat)
    {
      throw std::runtime_error("Could not find seat");
    }
    wlShellSurface = wl_shell_get_shell_surface(wlShell, wlSurface);
    if (!wlShellSurface)
    {
      throw std::runtime_error("Could not get shell surface");
    }
    addListener(wlShellSurface, *this);
    wl_shell_surface_set_toplevel(wlShellSurface);
    wl_display_roundtrip(wlDisplay);
  }

  void WaylandSurface::shellSurfacePing(struct wl_shell_surface *shellSurface, uint32_t serial)
  {
    wl_shell_surface_pong(shellSurface, serial);
  }

  void WaylandSurface::shellSurfaceConfigure(struct wl_shell_surface *shellSurface, uint32_t edges, int32_t width, int32_t height)
  {
    std::cout << "shell_surface_configure" << std::endl;
  }

  void WaylandSurface::shellSurfacePopupDone(struct wl_shell_surface *shellSurface)
  {
  }

  void WaylandSurface::registryAddObject(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
  {
    if (!strcmp(interface,"wl_compositor"))
      {
      	wlCompositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
      	wlSurface = wl_compositor_create_surface(wlCompositor);
      	if (!wlSurface)
        {
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
	addListener(wlSeat, *seatListener);
      }
  }

  void WaylandSurface::registryRemoveObject(struct wl_registry *registry, uint32_t name)
  {
  }

  void WaylandSurface::dispatch()
  {
    wl_display_dispatch_pending(wlDisplay);
  }

  bool WaylandSurface::isRunning() const
  {
    return seatListener->getRunning();
  }

}
