#pragma once

#include <wayland-server.h>

#include <memory>
#include <variant>

#include "protocol/ShmPool.hpp"

namespace protocol
{
  struct ShmBuffer
  {
    std::shared_ptr<ShmPool::Data> data;
    void destroy(wl_client *, wl_resource *);

    int offset;
    int width;
    int height;
    int stride;
    uint32_t format;
  };

  class Buffer
  {
  public:
    std::variant<ShmBuffer> data;

    void destroy(wl_client *, wl_resource *);
  };
}
