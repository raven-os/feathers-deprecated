#pragma once

#include <array>
#include <variant>

#include <magma/Image.hpp>
#include <magma/DeviceMemory.hpp>

namespace protocol
{
  class ShellSurface;
}

struct wl_resource;

namespace wm
{
  struct Rect
  {
    std::array<uint16_t, 2u> position;
    std::array<uint16_t, 2u> size;
  };

  struct ClientData
  {
    std::variant<protocol::ShellSurface *> data;
  };

  struct Tilling
  {
    static constexpr bool const horizontalTiling{false};
    static constexpr bool const verticalTiling{!horizontalTiling};
    bool direction{horizontalTiling};
    std::vector<wl_resource *> childResources;
  };

  struct Container
  {
    std::variant<Tilling> data;
  };

  struct WindowData
  {
    Rect rect;
    bool isSolid;
    std::variant<Container, ClientData> data;

    bool isVisible() const noexcept
    {
      return std::holds_alternative<ClientData>(data);
    }
  };
};
