#pragma once

#include <wayland-server.h>

namespace protocol
{
  class KeyboardImplem
  {
    public:
      KeyboardImplem() = delete;

      void release(wl_client *client, wl_resource *resource);
  };
}
