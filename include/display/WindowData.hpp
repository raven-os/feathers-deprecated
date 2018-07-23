#pragma once

#include <array>

#include <magma/Image.hpp>
#include <magma/DeviceMemory.hpp>

namespace display
{
  struct Rect
  {
    std::array<uint16_t, 2u> position;
    std::array<uint16_t, 2u> size;
  };
 
  struct WindowData
  {
    Rect rect;
    bool isSolid;
    //bool relative; TODO
  };

};
