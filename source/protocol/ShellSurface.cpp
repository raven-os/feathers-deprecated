#include <cassert>
#include <cstdio>

#include "protocol/ShellSurface.hpp"
#include "protocol/Surface.hpp"
#include "display/WindowTree.hpp"

namespace protocol
{
  ShellSurface::ShellSurface(Surface *surface, display::WindowTree *windowTree)
    : surface(surface)
    , windowTree(windowTree)
  {
    this->surface->setRole(this);
  }

  void ShellSurface::commit()
  {
    std::puts("commiting shell surface!");
  }

  // wl interface functions
  void ShellSurface::pong([[maybe_unused]] struct wl_client *client,
			  [[maybe_unused]] struct wl_resource *resource,
			  [[maybe_unused]] uint32_t serial)
  {
  }

  void ShellSurface::move([[maybe_unused]] struct wl_client *client,
			  [[maybe_unused]] struct wl_resource *resource,
			  [[maybe_unused]] struct wl_resource *seat,
			  [[maybe_unused]] uint32_t serial)
  {
  }
  
  void ShellSurface::resize([[maybe_unused]] struct wl_client *client,
			    [[maybe_unused]] struct wl_resource *resource,
			    [[maybe_unused]] struct wl_resource *seat,
			    [[maybe_unused]] uint32_t serial,
			    [[maybe_unused]] uint32_t edges)
  {
  }
  
  void ShellSurface::set_toplevel([[maybe_unused]] struct wl_client *client,
				  struct wl_resource *resource)
  {
    auto parentIndex(windowTree->getRootIndex());
    windowNodeIndex = windowTree->addChild(parentIndex);
    auto &data(windowTree->getData(windowNodeIndex));
    auto &parentData(windowTree->getData(parentIndex));

    data.isSolid = true;
    data.data = display::ClientData{this};
    std::visit([&](auto &containerData)
	       {
		 containerData.childResources.emplace_back(resource);
		 auto childIndexIt{windowTree->getChildren(parentIndex).begin()};

		 for (size_t i(0u); i != containerData.childResources.size(); ++i)
		   {
		     auto childResource(containerData.childResources[i]);
		     auto &childData(windowTree->getData(*childIndexIt));
		     auto x((parentData.rect.size[containerData.direction] * i) / containerData.childResources.size());
		     auto nextX((parentData.rect.size[containerData.direction] * (i + 1)) / containerData.childResources.size());
	
		     childData.rect.position[containerData.direction] = uint16_t(parentData.rect.position[containerData.direction] + x);
		     childData.rect.size[containerData.direction] = uint16_t(nextX - x);
		     childData.rect.position[!containerData.direction] = parentData.rect.position[!containerData.direction];
		     childData.rect.size[!containerData.direction] = parentData.rect.size[!containerData.direction];
		     wl_shell_surface_send_configure(childResource, 0, childData.rect.size[0], childData.rect.size[1]);
		     ++childIndexIt;
		   }
	       }, std::get<display::Container>(parentData.data).data);
  }
  
  void ShellSurface::set_transient([[maybe_unused]] struct wl_client *client,
				   [[maybe_unused]] struct wl_resource *resource,
				   [[maybe_unused]] struct wl_resource *parent,
				   [[maybe_unused]] int32_t x,
				   [[maybe_unused]] int32_t y,
				   [[maybe_unused]] uint32_t flags)
  {
  }
  
  void ShellSurface::set_fullscreen([[maybe_unused]] struct wl_client *client,
				    [[maybe_unused]] struct wl_resource *resource,
				    [[maybe_unused]] uint32_t method,
				    [[maybe_unused]] uint32_t framerate,
				    [[maybe_unused]] struct wl_resource *output)
  {
  }

  void ShellSurface::set_popup([[maybe_unused]] struct wl_client *client,
			       [[maybe_unused]] struct wl_resource *resource,
			       [[maybe_unused]] struct wl_resource *seat,
			       [[maybe_unused]] uint32_t serial,
			       [[maybe_unused]] struct wl_resource *parent,
			       [[maybe_unused]] int32_t x,
			       [[maybe_unused]] int32_t y,
			       [[maybe_unused]] uint32_t flags)
  {
  }

  void ShellSurface::set_maximized([[maybe_unused]] struct wl_client *client,
				   [[maybe_unused]] struct wl_resource *resource,
				   [[maybe_unused]] struct wl_resource *output)
  {
  }
  
  void ShellSurface::set_title(struct wl_client *,
			       struct wl_resource *,
			       const char *title)
  {
    this->title = title;
  }

  void ShellSurface::set_class(struct wl_client *,
			       struct wl_resource *,
			       const char *class_)
  {
    this->class_ = class_;
  }  
}
