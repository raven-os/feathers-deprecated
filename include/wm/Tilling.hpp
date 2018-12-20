#pragma once

#include <vector>

#include "wm/WindowNodeIndex.hpp"

struct wl_resource;

namespace wm
{
  class WindowTree;
  class WindowData;

  struct Tilling
  {
    static constexpr bool const horizontalTiling{false};
    static constexpr bool const verticalTiling{!horizontalTiling};
    bool direction{horizontalTiling};
    std::vector<wl_resource *> childResources;

    void recalculateChildren(WindowNodeIndex index, WindowTree &windowTree, WindowData const &windowData);
  };
}
