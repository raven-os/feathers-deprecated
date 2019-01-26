#pragma once

#include <wayland-server.h>

#include <memory>
#include <variant>

#include <magma/Image.hpp>
#include <magma/DeviceMemory.hpp>
#include <magma/ImageView.hpp>

#include "protocol/ShmPool.hpp"

namespace display
{
  class Renderer;
}

namespace protocol
{
  struct ShmBuffer
  {
    std::shared_ptr<ShmPool::Data> data;

    void destroy(wl_client *, wl_resource *);

    /// copy buffer into gpu memory
    void update(display::Renderer *renderer);

    int offset;
    int width;
    int height;
    int stride;
    uint32_t format;
    magma::Image<> image{};
    magma::DeviceMemory<> imageMemory{};
    magma::ImageView<> imageView{};
  };

  class Buffer
  {
  public:
    std::variant<ShmBuffer> data;

    void destroy(wl_client *, wl_resource *);
  };
}
