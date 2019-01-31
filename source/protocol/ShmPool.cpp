#include "protocol/ShmPool.hpp"
#include "protocol/Buffer.hpp"
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"

#include <sys/mman.h>
#include <cassert>
#include <cstring>
#include <cstdio>

namespace
{
  char *mapData(int fd, int size)
  {
    return static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_SHARED,
				    fd, 0));
  }
}

namespace protocol
{
  ShmPool::ShmPool(int fd, int size)
    : fd(fd)
    , data(new Data{mapData(fd, size), size})
      
  {
    assert(data->data); // TODO: add proper error handling (post 2 on failed mapping)
  }

  ShmPool::Data::~Data() noexcept
  {
    if (data)
      munmap(data, size);
  }

  void ShmPool::create_buffer(wl_client *client,
			      wl_resource *,
			      uint32_t id, int offset, int width, int height, int stride, uint32_t format)
  {
    assert(data);
    assert(offset + stride * height <= data->size); // TODO: check format and post error 0 or 1
    static auto buffer_implementation(createImplementation<struct wl_buffer_interface,
				      &Buffer::destroy>());

    instantiateImplementation(client, 1, id, wl_buffer_interface, &buffer_implementation,
			      new Buffer{ShmBuffer{data, offset, width, height, stride, format}},
			      [](wl_resource *resource)
			      {
				auto *buffer(static_cast<Buffer *>(wl_resource_get_user_data(resource)));
				buffer->resource_gone = true;

				if (!buffer->refCount)
				  delete buffer;
				else
				  --buffer->refCount;
			      });

  }

  void ShmPool::destroy(wl_client *,
			wl_resource *)
  {
    assert(data);
    data = nullptr;
  }

  void ShmPool::resize(wl_client *,
		       wl_resource *,
		       int newSize)
  {
    assert(data);
    *data = Data{mapData(fd, newSize), newSize};
  }
}
