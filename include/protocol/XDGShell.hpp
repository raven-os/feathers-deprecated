#pragma once

#include <wayland-server.h>

namespace wm
{
  class WindowTree;
}

namespace protocol
{
  class XDGShell
  {
    wm::WindowTree &windowTree;
  public:
    XDGShell(wm::WindowTree &windowTree);
    XDGShell(XDGShell const &) = delete;
    XDGShell(XDGShell &&) = delete;
    
    /**
     * destroy xdg_shell
     *
     * Destroy this xdg_shell object.
     *
     * Destroying a bound xdg_shell object while there are surfaces
     * still alive created by this xdg_shell object instance is illegal
     * and will result in a protocol error.
     */
    void destroy(struct wl_client *,
		 struct wl_resource *);

    /**
     * create a positioner object
     *
     * Create a positioner object. A positioner object is used to
     * position surfaces relative to some parent surface. See the
     * interface description and xdg_surface.get_popup for details.
     */
    void create_positioner(struct wl_client *,
			   struct wl_resource *,
			   uint32_t id);

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
    void get_xdg_surface(struct wl_client *,
			 struct wl_resource *,
			 uint32_t id,
			 struct wl_resource *surface);

    /**
     * respond to a ping event
     *
     * A client must respond to a ping event with a pong request or
     * the client may be deemed unresponsive. See xdg_shell.ping.
     * @param serial serial of the ping event
     */
    void pong(struct wl_client *client,
	      struct wl_resource *resource,
	      uint32_t serial);



  };
}
