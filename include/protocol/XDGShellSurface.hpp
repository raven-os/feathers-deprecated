#pragma once

#include "wm/WindowNodeIndex.hpp"
#include "generated/xdg-shell-unstable-v6-server-protocol.h"

#include <wayland-server.h>

#include <string>

namespace wm
{
  class WindowTree;
};

namespace protocol
{
  class Surface;

  class XDGShellSurface
  {
    Surface * const surface;
    wm::WindowTree * const windowTree;
    wm::WindowNodeIndex windowNodeIndex{wm::nullNode};
    std::string title;
    std::string class_;
    int32_t width;
    int32_t height;

  public:
    XDGShellSurface() = delete;
    XDGShellSurface(XDGShellSurface const &) = delete;
    XDGShellSurface(XDGShellSurface &&) = delete;

    XDGShellSurface(Surface *, wm::WindowTree *);

    void commit();
    void destroy();
  };
}

