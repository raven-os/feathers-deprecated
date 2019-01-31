#include "protocol/Surface.hpp"
#include "protocol/ShellSurface.hpp"
#include "protocol/XDGSurface.hpp"
#include "protocol/Buffer.hpp"
#include "protocol/CreateImplementation.hpp"

#include "magma/Buffer.hpp"
#include "display/Renderer.hpp"

#include "wm/WindowTree.hpp"

#include <iostream>

#include <wayland-server-protocol.h>

namespace protocol
{
  Surface::Surface(display::Renderer *renderer) noexcept
    : renderer(renderer)
  {
  }
  
  Surface::~Surface() noexcept
  {
    std::visit([](auto role)
	       {
		 role->surfaceDestroyed();
	       }, role);

  }


  void Surface::sendConfigure(wl_resource *resource, wm::Rect const &rect)
  {
    std::visit([&](auto role)
	       {
		 role->sendConfigure(resource, rect);
	       }, role);
  }

  void Surface::sendFrame()
  {
    if (frameCallback)
      {
	wl_callback_send_done(frameCallback, 1);
	frameCallback = nullptr;
      }
  }

  void Surface::destroy([[maybe_unused]] struct wl_client *client,
			[[maybe_unused]] struct wl_resource *resource)
  {
    std::visit([](auto role)
	       {
		 role->surfaceDestroyed();
	       }, role);
    role = NoRole{};
  }

  void Surface::attach([[maybe_unused]] struct wl_client *client,
		       [[maybe_unused]] struct wl_resource *resource,
		       struct wl_resource *buffer,
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
    std::cout << "client requesting callback" << std::endl;
    frameCallback = wl_resource_create(client, &wl_callback_interface, 1,
					callback);

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
    if (buffer)
      {
	std::visit([&](auto &buffer)
		   {
		     buffer.update(renderer);
		   }, static_cast<Buffer *>(wl_resource_get_user_data(buffer))->data);
      }
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
    assert(false);
  }

  void Surface::NoRole::surfaceDestroyed()
  {
  }

  void Surface::NoRole::sendConfigure(wl_resource *resource, wm::Rect const &rect)
  {
    assert(false);
  }
}
