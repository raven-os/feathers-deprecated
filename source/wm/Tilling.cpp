#include "wm/Tilling.hpp"
#include "wm/WindowTree.hpp"

#include <wayland-server.h>
#include "generated/xdg-shell-unstable-v6-server-protocol.h"

#include "protocol/Surface.hpp"

#include <iostream>

namespace wm
{
  void Tilling::recalculateChildren(WindowNodeIndex index, WindowTree &windowTree, WindowData const &windowData)
  {
    auto childIndexIt{windowTree.getChildren(index).begin()};

    for (size_t i(0u); i != childResources.size(); ++i)
      {
	auto childResource(childResources[i]);
	auto &childData(windowTree.getData(*childIndexIt));
	auto x((windowData.rect.size[direction] * i) / childResources.size());
	auto nextX((windowData.rect.size[direction] * (i + 1)) / childResources.size());
	
	childData.rect.position[direction] = uint16_t(windowData.rect.position[direction] + x);
	childData.rect.size[direction] = uint16_t(nextX - x);
	childData.rect.position[!direction] = windowData.rect.position[!direction];
	childData.rect.size[!direction] = windowData.rect.size[!direction];

	try
	  {
	    auto const &childSurfaceData(std::get<ClientData>(childData.data));

	    childSurfaceData.surface->sendConfigure(childResource, childData.rect);
	  }
	catch (std::bad_variant_access const &)
	  {}
	++childIndexIt;
      }
  }
}
