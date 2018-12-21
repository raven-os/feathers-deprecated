#pragma once

#include <wayland-server.h>
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"
#include "FeatherPointer.hpp"

namespace protocol
{
  class PointerImplem
  {

    public:
      PointerImplem() = delete;
      PointerImplem(PointerImplem const &) = delete;
      PointerImplem(PointerImplem &&) = delete;

      PointerImplem(wl_resource *);

      void set_cursor(wl_client *client, wl_resource *resource, uint32_t serial,
		      wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y);
      void release(wl_client *client, wl_resource *resource);

      void createImplem(wl_client *client, uint32_t id);

      FthPointer *fthPointer = nullptr;

    private:
      uint32_t version;
  };
}
