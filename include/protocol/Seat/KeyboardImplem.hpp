#pragma once

#include <wayland-server.h>
#include "Implem.hpp"

namespace protocol
{
  class KeyboardImplem : public Implem
  {
    public:
      KeyboardImplem() = delete;
      KeyboardImplem(KeyboardImplem const &) = delete;
      KeyboardImplem(KeyboardImplem &&) = delete;

      KeyboardImplem(wl_resource *);

      void createImplem(wl_client *client, uint32_t id) override;

    private:
      void sendKeymap(FthKeyboard *keyboard);
      void sendRepeatInfo(FthKeyboard *keyboard);
  };
}
