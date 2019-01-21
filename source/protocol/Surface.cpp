#include "protocol/Surface.hpp"
#include "protocol/ShellSurface.hpp"
#include "protocol/XDGSurface.hpp"
#include "wm/WindowTree.hpp"

namespace protocol
{
  void Surface::destroy([[maybe_unused]] struct wl_client *client,
			[[maybe_unused]] struct wl_resource *resource)
  {
    std::visit([](auto role)
	       {
		 role->surfaceDestroyed();
	       }, role);
  }

  void Surface::attach([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource,
		       [[maybe_unused]] struct wl_resource *buffer,
		       [[maybe_unused]] int32_t x,
		       [[maybe_unused]] int32_t y)
  {
    this->buffer = buffer;
  }
    
  void Surface::damage([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource,
		       [[maybe_unused]] int32_t x,
		       [[maybe_unused]] int32_t y,
		       [[maybe_unused]] int32_t width,
		       [[maybe_unused]] int32_t height)
  {
  }
    
  void Surface::frame([[maybe_unused]] struct wl_client *client,
		      [[maybe_unused]] struct wl_resource *resource,
		      [[maybe_unused]] uint32_t callback)
  {
  }

  void Surface::set_opaque_region([[maybe_unused]] struct wl_client *client,
				  [[maybe_unused]] struct wl_resource *resource,
				  [[maybe_unused]] struct wl_resource *region)
  {
  }

  void Surface::set_input_region([[maybe_unused]] struct wl_client *client,
				 [[maybe_unused]] struct wl_resource *resource,
				 [[maybe_unused]] struct wl_resource *region)
  {
  }

  void Surface::commit([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource)
  {
    std::visit([](auto role)
	       {
		 role->commit();
	       }, role);
  }

  void Surface::set_buffer_transform([[maybe_unused]] struct wl_client *client,
				     [[maybe_unused]] struct wl_resource *resource,
				     int32_t transform)
  {
    this->transform = wl_output_transform(transform);
  }

  void Surface::set_buffer_scale([[maybe_unused]] struct wl_client *client,
				 [[maybe_unused]] struct wl_resource *resource,
				 int32_t scale)
  {
    this->scale = scale;
  }


  void Surface::damage_buffer([[maybe_unused]] struct wl_client *client,
			      [[maybe_unused]] struct wl_resource *resource,
			      [[maybe_unused]] int32_t x,
			      [[maybe_unused]] int32_t y,
			      [[maybe_unused]] int32_t width,
			      [[maybe_unused]] int32_t height)
  {
  }

  void Surface::NoRole::commit()
  {
  }

  void Surface::NoRole::surfaceDestroyed()
  {
  }
}
