#include <cassert>
#include <cstdio>

#include "protocol/ShellSurface.hpp"
#include "protocol/Surface.hpp"
#include "wm/WindowTree.hpp"

namespace protocol
{
  ShellSurface::ShellSurface(Surface *surface, wm::WindowTree *windowTree)
    : surface(surface)
    , windowTree(windowTree)
  {
    this->surface->setRole(this);
  }

  void ShellSurface::commit()
  {
    std::puts("commiting shell surface!");
  }

  void ShellSurface::surfaceDestroyed()
  {
    auto parentIndex(windowTree->getParent(windowNodeIndex));

    windowTree->removeIndex(windowNodeIndex);

    auto &parentData(windowTree->getData(parentIndex));

    std::visit([&](auto &containerData)
	       {
		 containerData.childResources.erase(std::remove_if(containerData.childResources.begin(),
								   containerData.childResources.end(),
								   [this](wl_resource *resource) noexcept
								   {
								     return wl_resource_get_user_data(resource) == this;
								   })); // TODO: refactor into clean function
	       }, std::get<wm::Container>(parentData.data).data);
    parentData.recalculateChildren(parentIndex, *windowTree);
  }

  void ShellSurface::sendConfigure(wl_resource *resource, wm::Rect const &rect)
  {
    wl_shell_surface_send_configure(resource, 0, rect.size[0], rect.size[1]);
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
    data.data = wm::ClientData{this};
    std::visit([&](auto &containerData)
	       {
		 containerData.childResources.emplace_back(resource); // TODO: refactor into clean function
	       }, std::get<wm::Container>(parentData.data).data);
    parentData.recalculateChildren(parentIndex, *windowTree);
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
