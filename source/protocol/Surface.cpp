#include "protocol/Surface.hpp"

namespace protocol
{
  void Surface::destroy(struct wl_client *client,
			struct wl_resource *resource){}

  void Surface::attach(struct wl_client *client,
		       struct wl_resource *resource,
		       struct wl_resource *buffer,
		       int32_t x,
		       int32_t y){}
    
  void Surface::damage(struct wl_client *client,
		       struct wl_resource *resource,
		       int32_t x,
		       int32_t y,
		       int32_t width,
		       int32_t height){}
    
  void Surface::frame(struct wl_client *client,
		      struct wl_resource *resource,
		      uint32_t callback){}

  void Surface::set_opaque_region(struct wl_client *client,
				  struct wl_resource *resource,
				  struct wl_resource *region){}

  void Surface::set_input_region(struct wl_client *client,
				 struct wl_resource *resource,
				 struct wl_resource *region){}

  void Surface::commit(struct wl_client *client,
		       struct wl_resource *resource){}

  void Surface::set_buffer_transform(struct wl_client *client,
				     struct wl_resource *resource,
				     int32_t transform){}

  void Surface::set_buffer_scale(struct wl_client *client,
				 struct wl_resource *resource,
				 int32_t scale){}


  void Surface::damage_buffer(struct wl_client *client,
			      struct wl_resource *resource,
			      int32_t x,
			      int32_t y,
			      int32_t width,
			      int32_t height){}
}
