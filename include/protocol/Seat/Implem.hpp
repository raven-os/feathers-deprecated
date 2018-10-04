#pragma once

#include <wayland-server.h>
#include <iostream>
#include "FeatherSeat.hpp"
#include "protocol/CreateImplementation.hpp"

namespace protocol
{
  class Implem
  {
    protected:
      FthSeatClient *seat_client;
      uint32_t version;

    public:
      Implem() = delete;
      Implem(Implem const &) = delete;
      Implem(Implem &&) = delete;

      Implem(wl_resource *);

      void release(wl_client *client, wl_resource *resource);
      virtual void createImplem(wl_client *client, uint32_t id) = 0;


      FthSeatClient *getSeatClient();
      uint32_t getVersion();
  };
}
