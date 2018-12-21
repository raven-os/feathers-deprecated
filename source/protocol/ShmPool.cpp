#include "protocol/ShmPool.hpp"
#include "protocol/Buffer.hpp"
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"

#include <sys/mman.h>
#include <cassert>
#include <cstring>

namespace
{
  void *mapData(int fd, int size)
  {
    return mmap(nullptr, size, PROT_READ, MAP_SHARED,
		fd, 0);
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
    assert(offset + stride * height < data->size); // TODO: check format and post error 0 or 1
    static auto buffer_implementation(createImplementation<struct wl_buffer_interface,
				      &Buffer::destroy>());

    instantiateImplementation(client, 1, id, wl_buffer_interface, &buffer_implementation,
			      new Buffer{ShmBuffer{data, offset, width, height, stride, format}},
			      [](wl_resource *resource)
			      {
				delete static_cast<Buffer *>(wl_resource_get_user_data(resource));
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
    Data newData{mapData(fd, newSize), newSize};

    std::memcpy(newData.data, data->data, data->size);
    *data = newData;
  }
}
