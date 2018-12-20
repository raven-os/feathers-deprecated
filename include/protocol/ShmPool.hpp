#pragma once

#include <wayland-server.h>

namespace protocol
{
  class ShmPool
  {
  public:
    ShmPool(int fd, int size);

    void create_buffer(wl_client *,
		       wl_resource *,
		       uint32_t id, int offset, int width, int height, int stride, uint32_t format);
    void destroy(wl_client *,
		 wl_resource *);
    void resize(wl_client *,
		wl_resource *,
		int newSize);
  };
}
