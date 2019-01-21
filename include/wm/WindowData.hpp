#pragma once

#include <array>
#include <variant>

#include <magma/Image.hpp>
#include <magma/DeviceMemory.hpp>

#include "wm/Tilling.hpp"

namespace protocol
{
  class ShellSurface;
  class XDGSurface;
}

namespace wm
{
  struct Rect
  {
    std::array<uint16_t, 2u> position;
    std::array<uint16_t, 2u> size;
  };

  struct ClientData
  {
    std::variant<protocol::ShellSurface *, protocol::XDGSurface *> data;
  };

  struct Container
  {
    std::variant<Tilling> data;

    void recalculateChildren(WindowNodeIndex index, WindowTree &windowTree, WindowData const &windowData);
  };

  struct WindowData
  {
    Rect rect;
    bool isSolid;
    std::variant<Container, ClientData> data;

    void recalculateChildren(WindowNodeIndex index, WindowTree &windowTree);

    bool isVisible() const noexcept
    {
      return std::holds_alternative<ClientData>(data);
    }
  };
};
