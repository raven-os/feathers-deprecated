#include <cassert>
#include <cstdio>
#include <algorithm>

#include "protocol/XDGSurface.hpp"
#include "protocol/XDGTopLevel.hpp"
#include "protocol/Surface.hpp"

#include "wm/WindowTree.hpp"

#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"

#include "generated/xdg-shell-unstable-v6-server-protocol.h"

namespace protocol
{
  XDGSurface::XDGSurface(Surface *surface, wm::WindowTree *windowTree)
    : surface(surface)
    , windowTree(windowTree)
  {
    this->surface->setRole(this);
  }

  void XDGSurface::commit()
  {
    std::puts("commiting shell surface!");
  }

  void XDGSurface::surfaceDestroyed()
  {
    auto parentIndex(windowTree->getParent(windowNodeIndex));

    windowTree->removeIndex(windowNodeIndex);

    std::visit([&](auto &containerData)
	       {
		 containerData.childResources.erase(std::remove_if(containerData.childResources.begin(),
								   containerData.childResources.end(),
								   [this](wl_resource *resource) noexcept
								   {
								     return wl_resource_get_user_data(resource) == this;
								   })); // TODO: refactor into clean function
	       }, std::get<wm::Container>(windowTree->getData(parentIndex).data).data);
    windowTree->getData(parentIndex).recalculateChildren(parentIndex, *windowTree);
  }
  
  void XDGSurface::sendConfigure(wl_resource *resource, wm::Rect const &rect)
  {
    struct wl_array states;

    wl_array_init(&states);
    *reinterpret_cast<uint32_t *>(wl_array_add(&states, sizeof(uint32_t))) = ZXDG_TOPLEVEL_V6_STATE_RESIZING;
    zxdg_toplevel_v6_send_configure(topLevelResource, rect.size[0], rect.size[1], &states);
    zxdg_surface_v6_send_configure(resource, 0);
  }


  /**
   * destroy the xdg_surface
   *
   * Destroy the xdg_surface object. An xdg_surface must only be
   * destroyed after its role object has been destroyed.
   */
  void XDGSurface::destroy(struct wl_client *client,
			   struct wl_resource *resource)
  {
  }

  /**
   * assign the xdg_toplevel surface role
   *
   * This creates an xdg_toplevel object for the given xdg_surface
   * and gives the associated wl_surface the xdg_toplevel role.
   *
   * See the documentation of xdg_toplevel for more details about
   * what an xdg_toplevel is and how it is used.
   */
  void XDGSurface::get_toplevel(struct wl_client *client,
				struct wl_resource *resource,
				uint32_t id)
  {
    auto parentIndex(windowTree->getRootIndex());

    windowNodeIndex = windowTree->addChild(parentIndex);
    
    auto &data(windowTree->getData(windowNodeIndex));
    auto &parentData(windowTree->getData(parentIndex));

    data.isSolid = true;
    data.data = wm::ClientData{surface};

    static auto toplevel_implementation(createImplementation<struct zxdg_toplevel_v6_interface,
					&XDGTopLevel::destroy,
					&XDGTopLevel::set_parent,
					&XDGTopLevel::set_title,
					&XDGTopLevel::set_app_id,
					&XDGTopLevel::show_window_menu,
					&XDGTopLevel::move,
					&XDGTopLevel::resize,
					&XDGTopLevel::set_max_size,
					&XDGTopLevel::set_min_size,
					&XDGTopLevel::set_maximized,
					&XDGTopLevel::unset_maximized,
					&XDGTopLevel::set_fullscreen,
					&XDGTopLevel::unset_fullscreen,
					&XDGTopLevel::set_minimized>());

    topLevelResource =
      instantiateImplementation(client, 1, id, zxdg_toplevel_v6_interface,  &toplevel_implementation, new XDGTopLevel(),
				[](wl_resource *resource)
				{
				  delete static_cast<XDGTopLevel *>(wl_resource_get_user_data(resource));
				});

    std::visit([&](auto &containerData)
	       {
		 containerData.childResources.emplace_back(resource); // TODO: refactor into clean function
	       }, std::get<wm::Container>(parentData.data).data);
    parentData.recalculateChildren(parentIndex, *windowTree);
  }

  /**
   * assign the xdg_popup surface role
   *
   * This creates an xdg_popup object for the given xdg_surface and
   * gives the associated wl_surface the xdg_popup role.
   *
   * See the documentation of xdg_popup for more details about what
   * an xdg_popup is and how it is used.
   */
  void XDGSurface::get_popup(struct wl_client *client,
			     struct wl_resource *resource,
			     uint32_t id,
			     struct wl_resource *parent,
			     struct wl_resource *positioner)
  {
  }

  /**
   * set the new window geometry
   *
   * The window geometry of a surface is its "visible bounds" from
   * the user's perspective. Client-side decorations often have
   * invisible portions like drop-shadows which should be ignored for
   * the purposes of aligning, placing and constraining windows.
   *
   * The window geometry is double buffered, and will be applied at
   * the time wl_surface.commit of the corresponding wl_surface is
   * called.
   *
   * Once the window geometry of the surface is set, it is not
   * possible to unset it, and it will remain the same until
   * set_window_geometry is called again, even if a new subsurface or
   * buffer is attached.
   *
   * If never set, the value is the full bounds of the surface,
   * including any subsurfaces. This updates dynamically on every
   * commit. This unset is meant for extremely simple clients.
   *
   * The arguments are given in the surface-local coordinate space of
   * the wl_surface associated with this xdg_surface.
   *
   * The width and height must be greater than zero. Setting an
   * invalid size will raise an error. When applied, the effective
   * window geometry will be the set window geometry clamped to the
   * bounding rectangle of the combined geometry of the surface of
   * the xdg_surface and the associated subsurfaces.
   */
  void XDGSurface::set_window_geometry(struct wl_client *client,
				       struct wl_resource *resource,
				       int32_t x,
				       int32_t y,
				       int32_t width,
				       int32_t height)
  {
  }

  /**
   * ack a configure event
   *
   * When a configure event is received, if a client commits the
   * surface in response to the configure event, then the client must
   * make an ack_configure request sometime before the commit
   * request, passing along the serial of the configure event.
   *
   * For instance, for toplevel surfaces the compositor might use
   * this information to move a surface to the top left only when the
   * client has drawn itself for the maximized or fullscreen state.
   *
   * If the client receives multiple configure events before it can
   * respond to one, it only has to ack the last configure event.
   *
   * A client is not required to commit immediately after sending an
   * ack_configure request - it may even ack_configure several times
   * before its next surface commit.
   *
   * A client may send multiple ack_configure requests before
   * committing, but only the last request sent before a commit
   * indicates which configure event the client really is responding
   * to.
   * @param serial the serial from the configure event
   */
  void XDGSurface::ack_configure(struct wl_client *client,
				 struct wl_resource *resource,
				 uint32_t serial)
  {
  }
}
