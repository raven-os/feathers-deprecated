#pragma once

#include <wayland-server.h>

#include <memory>

namespace protocol
{
  class ShmPool
  {
  public:
    struct Data
    {
      char *data;
      int size;
      
      ~Data() noexcept;
    };

  private:
    int const fd;
    std::shared_ptr<Data> data;

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
