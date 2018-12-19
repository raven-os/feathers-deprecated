#pragma once

#include <wayland-server.h>
#include "KeyboardImplem.hpp"
#include "PointerImplem.hpp"
#include "LibInput.hpp"

namespace protocol
{

  class SeatImplem
  {
  public:
    SeatImplem();

    void createImplem(wl_client *client, uint32_t version, uint32_t id);

    void get_pointer (wl_client *client,  wl_resource *resource, uint32_t id);
    void get_keyboard (wl_client *client,  wl_resource *resource, uint32_t id);
    void get_touch (wl_client *client,  wl_resource *resource, uint32_t id);
    void release (wl_client *client,  wl_resource *resource);

  private:
    wl_global *global;
    wl_list resources;

    char *name = "seat0";
    uint32_t capabilities;

    KeyboardImplem *keyboard = nullptr;
    PointerImplem *pointer = nullptr;

    LibInput input;

  private:
    void setCapabilitites();
    void setName();

  };
}
