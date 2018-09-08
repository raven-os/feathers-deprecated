#pragma once

#include <wayland-server.h>

namespace protocol
{
  class Surface
  {
    bool taken{false}; // surface already has a role
  public:
    class Taken{};

    void setTaken()
    {
      if (taken)
	throw Taken{};
      taken = true;
    }
      

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
