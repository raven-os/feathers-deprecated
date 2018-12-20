#include "protocol/ShmPool.hpp"

namespace protocol
{
  ShmPool::ShmPool(int fd, int size)
  {
  }

  void ShmPool::create_buffer(wl_client *,
			      wl_resource *,
			      uint32_t id, int offset, int width, int height, int stride, uint32_t format)
  {
  }

  void ShmPool::destroy(wl_client *,
			wl_resource *)
  {
  }

  void ShmPool::resize(wl_client *,
		       wl_resource *,
		       int newSize)
  {
  }
}
