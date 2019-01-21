#include "protocol/XDGShell.hpp"
#include "protocol/XDGSurface.hpp"
#include "protocol/Surface.hpp"
#include "protocol/CreateImplementation.hpp"
#include "protocol/InstantiateImplementation.hpp"

#include "generated/xdg-shell-unstable-v6-server-protocol.h"

namespace protocol
{
  XDGShell::XDGShell(wm::WindowTree &windowTree)
    : windowTree(windowTree)
  {
  }
    
  /**
   * destroy xdg_shell
   *
   * Destroy this xdg_shell object.
   *
   * Destroying a bound xdg_shell object while there are surfaces
   * still alive created by this xdg_shell object instance is illegal
   * and will result in a protocol error.
   */
  void XDGShell::destroy(struct wl_client *client,
			 struct wl_resource *resource)
  {
  }

  /**
   * create a positioner object
   *
   * Create a positioner object. A positioner object is used to
   * position surfaces relative to some parent surface. See the
   * interface description and xdg_surface.get_popup for details.
   */
  void XDGShell::create_positioner(struct wl_client *client,
				   struct wl_resource *resource,
				   uint32_t id)
  {
  }

  /**
   * create a shell surface from a surface
   *
   * This creates an xdg_surface for the given surface. While
   * xdg_surface itself is not a role, the corresponding surface may
   * only be assigned a role extending xdg_surface, such as
   * xdg_toplevel or xdg_popup.
   *
   * This creates an xdg_surface for the given surface. An
   * xdg_surface is used as basis to define a role to a given
   * surface, such as xdg_toplevel or xdg_popup. It also manages
   * functionality shared between xdg_surface based surface roles.
   *
   * See the documentation of xdg_surface for more details about what
   * an xdg_surface is and how it is used.
   */
  void XDGShell::get_xdg_surface(struct wl_client *client,
				 struct wl_resource *resource,
				 uint32_t id,
				 struct wl_resource *surfaceResource)
  {
    Surface *surface(static_cast<Surface *>(wl_resource_get_user_data(surfaceResource)));

    try
      {
	static auto zxdg_surface_v6_interface_implementation(createImplementation<struct zxdg_surface_v6_interface,
							     &XDGSurface::destroy,
							     &XDGSurface::get_toplevel,
							     &XDGSurface::get_popup,
							     &XDGSurface::set_window_geometry,
							     &XDGSurface::ack_configure>());

	instantiateImplementation(client, 1, id, zxdg_surface_v6_interface, &zxdg_surface_v6_interface_implementation, new XDGSurface(surface, &windowTree),
				  [](wl_resource *resource)
				  {
				    delete static_cast<XDGSurface *>(wl_resource_get_user_data(resource));
				  });
      }
    catch (Surface::Taken)
      {
	wl_resource_post_error(surfaceResource, WL_SHELL_ERROR_ROLE, "");
      }

  }

  /**
   * respond to a ping event
   *
   * A client must respond to a ping event with a pong request or
   * the client may be deemed unresponsive. See xdg_shell.ping.
   * @param serial serial of the ping event
   */
  void XDGShell::pong(struct wl_client *client,
		      struct wl_resource *resource,
		      uint32_t serial)
  {
  }

}
