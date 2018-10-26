#pragma once

#include "display/WindowTree.hpp"

#include <wayland-server.h>

#include <string>

namespace display
{
  class WindowTree;
};

namespace protocol
{
  class Surface;

  class ShellSurface
  {
    Surface * const surface;
    display::WindowTree * const windowTree;
    display::WindowTree::WindowNodeIndex windowNodeIndex{display::WindowTree::nullNode};
    std::string title;
    std::string class_;
    int32_t width;
    int32_t height;

  public:
    ShellSurface() = delete;
    ShellSurface(ShellSurface const &) = delete;
    ShellSurface(ShellSurface &&) = delete;

    ShellSurface(Surface *, display::WindowTree *);

    void commit();

    // wl interface functions
    void pong(struct wl_client *client,
	      struct wl_resource *resource,
	      uint32_t serial);
    void move(struct wl_client *client,
	      struct wl_resource *resource,
	      struct wl_resource *seat,
	      uint32_t serial);
    void resize(struct wl_client *client,
		struct wl_resource *resource,
		struct wl_resource *seat,
		uint32_t serial,
		uint32_t edges);
    void set_toplevel(struct wl_client *client,
		      struct wl_resource *resource);
    void set_transient(struct wl_client *client,
		       struct wl_resource *resource,
		       struct wl_resource *parent,
		       int32_t x,
		       int32_t y,
		       uint32_t flags);
    void set_fullscreen(struct wl_client *client,
			struct wl_resource *resource,
			uint32_t method,
			uint32_t framerate,
			struct wl_resource *output);
    void set_popup(struct wl_client *client,
		   struct wl_resource *resource,
		   struct wl_resource *seat,
		   uint32_t serial,
		   struct wl_resource *parent,
		   int32_t x,
		   int32_t y,
		   uint32_t flags);
    void set_maximized(struct wl_client *client,
		       struct wl_resource *resource,
		       struct wl_resource *output);
    void set_title(struct wl_client *client,
		   struct wl_resource *resource,
		   const char *title);
    void set_class(struct wl_client *client,
		   struct wl_resource *resource,
		   const char *class_);
  };
}
