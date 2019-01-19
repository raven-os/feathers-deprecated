#pragma once

#include <wayland-server.h>
#include <variant>

namespace protocol
{
  class ShellSurface;
  class XDGSurface;

  class Surface
  {
    wl_output_transform transform{wl_output_transform::WL_OUTPUT_TRANSFORM_NORMAL};
    struct wl_resource *buffer{nullptr};
    int32_t scale{1};
    class NoRole
    {
    public:
      NoRole *operator->()
      {
	return this;
      }

      void commit();
      void surfaceDestroyed();
    };
    std::variant<NoRole, ShellSurface *, XDGSurface *> role{NoRole{}};
  public:
    class Taken{};

    Surface() = default;
    Surface(Surface const &) = delete;
    Surface(Surface &&) = delete;

    template<class Role>
    void setRole(Role *role)
    {
      if (!std::holds_alternative<NoRole>(this->role))
	throw Taken{};
      this->role = role;
    }

  public:
    // wl interface functions
    void destroy(struct wl_client *client,
		 struct wl_resource *resource);

    void attach(struct wl_client *client,
		struct wl_resource *resource,
		struct wl_resource *buffer,
		int32_t x,
		int32_t y);
    
    void damage(struct wl_client *client,
		struct wl_resource *resource,
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height);
    
    void frame(struct wl_client *client,
	       struct wl_resource *resource,
	       uint32_t callback);

    void set_opaque_region(struct wl_client *client,
			   struct wl_resource *resource,
			   struct wl_resource *region);

    void set_input_region(struct wl_client *client,
			  struct wl_resource *resource,
			  struct wl_resource *region);

    void commit(struct wl_client *client,
		struct wl_resource *resource);

    void set_buffer_transform(struct wl_client *client,
			      struct wl_resource *resource,
			      int32_t transform);

    void set_buffer_scale(struct wl_client *client,
			  struct wl_resource *resource,
			  int32_t scale);


    void damage_buffer(struct wl_client *client,
		       struct wl_resource *resource,
		       int32_t x,
		       int32_t y,
		       int32_t width,
		       int32_t height);
  };
}
