#pragma once

#include <cstdint>
#include <claws/utils/tagged_data.hpp>

namespace wm
{
  struct WindowNodeTag;

  using WindowNodeIndex = claws::tagged_data<uint16_t, uint16_t, WindowNodeTag>;

  static constexpr WindowNodeIndex nullNode{uint16_t(-1u)};
}
