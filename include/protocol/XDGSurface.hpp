#pragma once

#include "wm/WindowNodeIndex.hpp"

#include <wayland-server.h>

#include <string>

namespace wm
{
  class WindowTree;
};

namespace protocol
{
  class Surface;

  class XDGSurface
  {
    Surface * const surface;
    wm::WindowTree * const windowTree;
    wm::WindowNodeIndex windowNodeIndex{wm::nullNode};
    std::string title;
    std::string class_;
    int32_t width;
    int32_t height;

  public:
    XDGSurface() = delete;
    XDGSurface(XDGSurface const &) = delete;
    XDGSurface(XDGSurface &&) = delete;

    XDGSurface(Surface *, wm::WindowTree *);

    void commit();
    void surfaceDestroyed();

    /**
     * destroy the xdg_surface
     *
     * Destroy the xdg_surface object. An xdg_surface must only be
     * destroyed after its role object has been destroyed.
     */
    void destroy(struct wl_client *client,
		 struct wl_resource *resource);
    /**
     * assign the xdg_toplevel surface role
     *
     * This creates an xdg_toplevel object for the given xdg_surface
     * and gives the associated wl_surface the xdg_toplevel role.
     *
     * See the documentation of xdg_toplevel for more details about
     * what an xdg_toplevel is and how it is used.
     */
    void get_toplevel(struct wl_client *client,
		      struct wl_resource *resource,
		      uint32_t id);
    /**
     * assign the xdg_popup surface role
     *
     * This creates an xdg_popup object for the given xdg_surface and
     * gives the associated wl_surface the xdg_popup role.
     *
     * See the documentation of xdg_popup for more details about what
     * an xdg_popup is and how it is used.
     */
    void get_popup(struct wl_client *client,
		   struct wl_resource *resource,
		   uint32_t id,
		   struct wl_resource *parent,
		   struct wl_resource *positioner);
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
    void set_window_geometry(struct wl_client *client,
			     struct wl_resource *resource,
			     int32_t x,
			     int32_t y,
			     int32_t width,
			     int32_t height);
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
    void ack_configure(struct wl_client *client,
		       struct wl_resource *resource,
		       uint32_t serial);
  };
}
