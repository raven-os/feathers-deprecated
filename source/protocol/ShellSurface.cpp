#include <cassert>
#include <cstdio>

#include "protocol/ShellSurface.hpp"
#include "protocol/Surface.hpp"

namespace protocol
{
  ShellSurface::ShellSurface(Surface *surface)
    : surface(surface)
  {
    this->surface->setRole(this);
  }

  void ShellSurface::commit()
  {
    std::puts("commiting shell surface!");
  }

  // wl interface functions
  void ShellSurface::pong([[maybe_unused]] struct wl_client *client,
			  [[maybe_unused]] struct wl_resource *resource,
			  [[maybe_unused]] uint32_t serial)
  {
  }

  void ShellSurface::move([[maybe_unused]] struct wl_client *client,
			  [[maybe_unused]] struct wl_resource *resource,
			  [[maybe_unused]] struct wl_resource *seat,
			  [[maybe_unused]] uint32_t serial)
  {
  }
  
  void ShellSurface::resize([[maybe_unused]] struct wl_client *client,
			    [[maybe_unused]] struct wl_resource *resource,
			    [[maybe_unused]] struct wl_resource *seat,
			    [[maybe_unused]] uint32_t serial,
			    [[maybe_unused]] uint32_t edges)
  {
  }
  
  void ShellSurface::set_toplevel([[maybe_unused]] struct wl_client *client,
				  [[maybe_unused]] struct wl_resource *resource)
  {
  }
  
  void ShellSurface::set_transient([[maybe_unused]] struct wl_client *client,
				   [[maybe_unused]] struct wl_resource *resource,
				   [[maybe_unused]] struct wl_resource *parent,
				   [[maybe_unused]] int32_t x,
				   [[maybe_unused]] int32_t y,
				   [[maybe_unused]] uint32_t flags)
  {
  }
  
  void ShellSurface::set_fullscreen([[maybe_unused]] struct wl_client *client,
				    [[maybe_unused]] struct wl_resource *resource,
				    [[maybe_unused]] uint32_t method,
				    [[maybe_unused]] uint32_t framerate,
				    [[maybe_unused]] struct wl_resource *output)
  {
  }

  void ShellSurface::set_popup([[maybe_unused]] struct wl_client *client,
			       [[maybe_unused]] struct wl_resource *resource,
			       [[maybe_unused]] struct wl_resource *seat,
			       [[maybe_unused]] uint32_t serial,
			       [[maybe_unused]] struct wl_resource *parent,
			       [[maybe_unused]] int32_t x,
			       [[maybe_unused]] int32_t y,
			       [[maybe_unused]] uint32_t flags)
  {
  }

  void ShellSurface::set_maximized([[maybe_unused]] struct wl_client *client,
				   [[maybe_unused]] struct wl_resource *resource,
				   [[maybe_unused]] struct wl_resource *output)
  {
  }
  
  void ShellSurface::set_title(struct wl_client *,
			       struct wl_resource *,
			       const char *title)
  {
    this->title = title;
  }

  void ShellSurface::set_class(struct wl_client *,
			       struct wl_resource *,
			       const char *class_)
  {
    this->class_ = class_;
  }  
}
