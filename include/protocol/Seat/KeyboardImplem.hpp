#pragma once

#include <wayland-server.h>
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"
#include "FeatherKeyboard.hpp"

namespace protocol
{
  class KeyboardImplem
  {
    public:
      KeyboardImplem() = delete;
      KeyboardImplem(KeyboardImplem const &) = delete;
      KeyboardImplem(KeyboardImplem &&) = delete;

      KeyboardImplem(wl_resource *resource);

      void createImplem(FthKeyboard *fthKeyboard, wl_client *client, uint32_t id);
      void release(wl_client *client, wl_resource *resource);

    private:
      uint32_t version;
  };
}
