#include "protocol/ShellSurface.hpp"
#include "protocol/Surface.hpp"

namespace protocol
{
  ShellSurface::ShellSurface(Surface *surface)
    : surface(surface)
  {}

  // wl interface functions
  void ShellSurface::pong(struct wl_client *client,
			  struct wl_resource *resource,
			  uint32_t serial)
  {
  }

  void ShellSurface::move(struct wl_client *client,
			  struct wl_resource *resource,
			  struct wl_resource *seat,
			  uint32_t serial)
  {
  }
  
  void ShellSurface::resize(struct wl_client *client,
			    struct wl_resource *resource,
			    struct wl_resource *seat,
			    uint32_t serial,
			    uint32_t edges)
  {
  }
  
  void ShellSurface::set_toplevel(struct wl_client *client,
				  struct wl_resource *resource)
  {
  }
  
  void ShellSurface::set_transient(struct wl_client *client,
				   struct wl_resource *resource,
				   struct wl_resource *parent,
				   int32_t x,
				   int32_t y,
				   uint32_t flags)
  {
  }
  
  void ShellSurface::set_fullscreen(struct wl_client *client,
				    struct wl_resource *resource,
				    uint32_t method,
				    uint32_t framerate,
				    struct wl_resource *output)
  {
  }

  void ShellSurface::set_popup(struct wl_client *client,
			       struct wl_resource *resource,
			       struct wl_resource *seat,
			       uint32_t serial,
			       struct wl_resource *parent,
			       int32_t x,
			       int32_t y,
			       uint32_t flags)
  {
  }

  void ShellSurface::set_maximized(struct wl_client *client,
				   struct wl_resource *resource,
				   struct wl_resource *output)
  {
  }
  
  void ShellSurface::set_title(struct wl_client *client,
			       struct wl_resource *resource,
			       const char *title)
  {
  }

  void ShellSurface::set_class(struct wl_client *client,
			       struct wl_resource *resource,
			       const char *class_)
  {
  }  
}
