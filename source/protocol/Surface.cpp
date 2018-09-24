#include "protocol/Surface.hpp"

namespace protocol
{
  void Surface::destroy([[maybe_unused]] struct wl_client *client,
			[[maybe_unused]] struct wl_resource *resource){}

  void Surface::attach([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource,
		       [[maybe_unused]] struct wl_resource *buffer,
		       [[maybe_unused]] int32_t x,
		       [[maybe_unused]] int32_t y){}
    
  void Surface::damage([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource,
		       [[maybe_unused]] int32_t x,
		       [[maybe_unused]] int32_t y,
		       [[maybe_unused]] int32_t width,
		       [[maybe_unused]] int32_t height){}
    
  void Surface::frame([[maybe_unused]] struct wl_client *client,
		      [[maybe_unused]] struct wl_resource *resource,
		      [[maybe_unused]] uint32_t callback){}

  void Surface::set_opaque_region([[maybe_unused]] struct wl_client *client,
				  [[maybe_unused]] struct wl_resource *resource,
				  [[maybe_unused]] struct wl_resource *region){}

  void Surface::set_input_region([[maybe_unused]] struct wl_client *client,
				 [[maybe_unused]] struct wl_resource *resource,
				 [[maybe_unused]] struct wl_resource *region){}

  void Surface::commit([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource){}

  void Surface::set_buffer_transform([[maybe_unused]] struct wl_client *client,
				     [[maybe_unused]] struct wl_resource *resource,
				     [[maybe_unused]] int32_t transform){}

  void Surface::set_buffer_scale([[maybe_unused]] struct wl_client *client,
				 [[maybe_unused]] struct wl_resource *resource,
				 [[maybe_unused]] int32_t scale){}


  void Surface::damage_buffer([[maybe_unused]] struct wl_client *client,
			      [[maybe_unused]] struct wl_resource *resource,
			      [[maybe_unused]] int32_t x,
			      [[maybe_unused]] int32_t y,
			      [[maybe_unused]] int32_t width,
			      [[maybe_unused]] int32_t height){}
}
