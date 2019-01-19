#include "protocol/Buffer.hpp"

#include <cassert>

namespace protocol
{
  void Buffer::destroy(wl_client *client, wl_resource *resource)
  {
    std::visit([client, resource](auto &data)
	       {
		 data.destroy(client, resource);
	       }, data);
  }

  void ShmBuffer::destroy(wl_client *, wl_resource *)
  {
    assert(data);
    data = nullptr;
  }
}
