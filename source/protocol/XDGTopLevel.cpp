#include "protocol/XDGTopLevel.hpp"

namespace protocol
{
  /**
   * destroy the xdg_toplevel
   *
   * Unmap and destroy the window. The window will be effectively
   * hidden from the user's point of view, and all state like
   * maximization, fullscreen, and so on, will be lost.
   */
  void XDGTopLevel::destroy(struct wl_client *client,
			    struct wl_resource *resource){}
  /**
   * set the parent of this surface
   *
   * Set the "parent" of this surface. This window should be
   * stacked above a parent. The parent surface must be mapped as
   * long as this surface is mapped.
   *
   * Parent windows should be set on dialogs, toolboxes, or other
   * "auxiliary" surfaces, so that the parent is raised when the
   * dialog is raised.
   */
  void XDGTopLevel::set_parent(struct wl_client *client,
			       struct wl_resource *resource,
			       struct wl_resource *parent){}
  /**
   * set surface title
   *
   * Set a short title for the surface.
   *
   * This string may be used to identify the surface in a task bar,
   * window list, or other user interface elements provided by the
   * compositor.
   *
   * The string must be encoded in UTF-8.
   */
  void XDGTopLevel::set_title(struct wl_client *client,
			      struct wl_resource *resource,
			      const char *title){}
  /**
   * set application ID
   *
   * Set an application identifier for the surface.
   *
   * The app ID identifies the general class of applications to which
   * the surface belongs. The compositor can use this to group
   * multiple surfaces together, or to determine how to launch a new
   * application.
   *
   * For D-Bus activatable applications, the app ID is used as the
   * D-Bus service name.
   *
   * The compositor shell will try to group application surfaces
   * together by their app ID. As a best practice, it is suggested to
   * select app ID's that match the basename of the application's
   * .desktop file. For example, "org.freedesktop.FooViewer" where
   * the .desktop file is "org.freedesktop.FooViewer.desktop".
   *
   * See the desktop-entry specification [0] for more details on
   * application identifiers and how they relate to well-known D-Bus
   * names and .desktop files.
   *
   * [0] http://standards.freedesktop.org/desktop-entry-spec/
   */
  void XDGTopLevel::set_app_id(struct wl_client *client,
			       struct wl_resource *resource,
			       const char *app_id){}
  /**
   * show the window menu
   *
   * Clients implementing client-side decorations might want to
   * show a context menu when right-clicking on the decorations,
   * giving the user a menu that they can use to maximize or minimize
   * the window.
   *
   * This request asks the compositor to pop up such a window menu at
   * the given position, relative to the local surface coordinates of
   * the parent surface. There are no guarantees as to what menu
   * items the window menu contains.
   *
   * This request must be used in response to some sort of user
   * action like a button press, key press, or touch down event.
   * @param seat the wl_seat of the user event
   * @param serial the serial of the user event
   * @param x the x position to pop up the window menu at
   * @param y the y position to pop up the window menu at
   */
  void XDGTopLevel::show_window_menu(struct wl_client *client,
				     struct wl_resource *resource,
				     struct wl_resource *seat,
				     uint32_t serial,
				     int32_t x,
				     int32_t y){}
  /**
   * start an interactive move
   *
   * Start an interactive, user-driven move of the surface.
   *
   * This request must be used in response to some sort of user
   * action like a button press, key press, or touch down event. The
   * passed serial is used to determine the type of interactive move
   * (touch, pointer, etc).
   *
   * The server may ignore move requests depending on the state of
   * the surface (e.g. fullscreen or maximized), or if the passed
   * serial is no longer valid.
   *
   * If triggered, the surface will lose the focus of the device
   * (wl_pointer, wl_touch, etc) used for the move. It is up to the
   * compositor to visually indicate that the move is taking place,
   * such as updating a pointer cursor, during the move. There is no
   * guarantee that the device focus will return when the move is
   * completed.
   * @param seat the wl_seat of the user event
   * @param serial the serial of the user event
   */
  void XDGTopLevel::move(struct wl_client *client,
			 struct wl_resource *resource,
			 struct wl_resource *seat,
			 uint32_t serial){}
  /**
   * start an interactive resize
   *
   * Start a user-driven, interactive resize of the surface.
   *
   * This request must be used in response to some sort of user
   * action like a button press, key press, or touch down event. The
   * passed serial is used to determine the type of interactive
   * resize (touch, pointer, etc).
   *
   * The server may ignore resize requests depending on the state of
   * the surface (e.g. fullscreen or maximized).
   *
   * If triggered, the client will receive configure events with the
   * "resize" state enum value and the expected sizes. See the
   * "resize" enum value for more details about what is required. The
   * client must also acknowledge configure events using
   * "ack_configure". After the resize is completed, the client will
   * receive another "configure" event without the resize state.
   *
   * If triggered, the surface also will lose the focus of the device
   * (wl_pointer, wl_touch, etc) used for the resize. It is up to the
   * compositor to visually indicate that the resize is taking place,
   * such as updating a pointer cursor, during the resize. There is
   * no guarantee that the device focus will return when the resize
   * is completed.
   *
   * The edges parameter specifies how the surface should be resized,
   * and is one of the values of the resize_edge enum. The compositor
   * may use this information to update the surface position for
   * example when dragging the top left corner. The compositor may
   * also use this information to adapt its behavior, e.g. choose an
   * appropriate cursor image.
   * @param seat the wl_seat of the user event
   * @param serial the serial of the user event
   * @param edges which edge or corner is being dragged
   */
  void XDGTopLevel::resize(struct wl_client *client,
			   struct wl_resource *resource,
			   struct wl_resource *seat,
			   uint32_t serial,
			   uint32_t edges){}
  /**
   * set the maximum size
   *
   * Set a maximum size for the window.
   *
   * The client can specify a maximum size so that the compositor
   * does not try to configure the window beyond this size.
   *
   * The width and height arguments are in window geometry
   * coordinates. See xdg_surface.set_window_geometry.
   *
   * Values set in this way are double-buffered. They will get
   * applied on the next commit.
   *
   * The compositor can use this information to allow or disallow
   * different states like maximize or fullscreen and draw accurate
   * animations.
   *
   * Similarly, a tiling window manager may use this information to
   * place and resize client windows in a more effective way.
   *
   * The client should not rely on the compositor to obey the maximum
   * size. The compositor may decide to ignore the values set by the
   * client and request a larger size.
   *
   * If never set, or a value of zero in the request, means that the
   * client has no expected maximum size in the given dimension. As a
   * result, a client wishing to reset the maximum size to an
   * unspecified state can use zero for width and height in the
   * request.
   *
   * Requesting a maximum size to be smaller than the minimum size of
   * a surface is illegal and will result in a protocol error.
   *
   * The width and height must be greater than or equal to zero.
   * Using strictly negative values for width and height will result
   * in a protocol error.
   */
  void XDGTopLevel::set_max_size(struct wl_client *client,
				 struct wl_resource *resource,
				 int32_t width,
				 int32_t height){}
  /**
   * set the minimum size
   *
   * Set a minimum size for the window.
   *
   * The client can specify a minimum size so that the compositor
   * does not try to configure the window below this size.
   *
   * The width and height arguments are in window geometry
   * coordinates. See xdg_surface.set_window_geometry.
   *
   * Values set in this way are double-buffered. They will get
   * applied on the next commit.
   *
   * The compositor can use this information to allow or disallow
   * different states like maximize or fullscreen and draw accurate
   * animations.
   *
   * Similarly, a tiling window manager may use this information to
   * place and resize client windows in a more effective way.
   *
   * The client should not rely on the compositor to obey the minimum
   * size. The compositor may decide to ignore the values set by the
   * client and request a smaller size.
   *
   * If never set, or a value of zero in the request, means that the
   * client has no expected minimum size in the given dimension. As a
   * result, a client wishing to reset the minimum size to an
   * unspecified state can use zero for width and height in the
   * request.
   *
   * Requesting a minimum size to be larger than the maximum size of
   * a surface is illegal and will result in a protocol error.
   *
   * The width and height must be greater than or equal to zero.
   * Using strictly negative values for width and height will result
   * in a protocol error.
   */
  void XDGTopLevel::set_min_size(struct wl_client *client,
				 struct wl_resource *resource,
				 int32_t width,
				 int32_t height){}
  /**
   * maximize the window
   *
   * Maximize the surface.
   *
   * After requesting that the surface should be maximized, the
   * compositor will respond by emitting a configure event with the
   * "maximized" state and the required window geometry. The client
   * should then update its content, drawing it in a maximized state,
   * i.e. without shadow or other decoration outside of the window
   * geometry. The client must also acknowledge the configure when
   * committing the new content (see ack_configure).
   *
   * It is up to the compositor to decide how and where to maximize
   * the surface, for example which output and what region of the
   * screen should be used.
   *
   * If the surface was already maximized, the compositor will still
   * emit a configure event with the "maximized" state.
   */
  void XDGTopLevel::set_maximized(struct wl_client *client,
				  struct wl_resource *resource){}
  /**
   * unmaximize the window
   *
   * Unmaximize the surface.
   *
   * After requesting that the surface should be unmaximized, the
   * compositor will respond by emitting a configure event without
   * the "maximized" state. If available, the compositor will include
   * the window geometry dimensions the window had prior to being
   * maximized in the configure request. The client must then update
   * its content, drawing it in a regular state, i.e. potentially
   * with shadow, etc. The client must also acknowledge the configure
   * when committing the new content (see ack_configure).
   *
   * It is up to the compositor to position the surface after it was
   * unmaximized{} usually the position the surface had before
   * maximizing, if applicable.
   *
   * If the surface was already not maximized, the compositor will
   * still emit a configure event without the "maximized" state.
   */
  void XDGTopLevel::unset_maximized(struct wl_client *client,
				    struct wl_resource *resource){}
  /**
   * set the window as fullscreen on a monitor
   *
   * Make the surface fullscreen.
   *
   * You can specify an output that you would prefer to be
   * fullscreen. If this value is NULL, it's up to the compositor to
   * choose which display will be used to map this surface.
   *
   * If the surface doesn't cover the whole output, the compositor
   * will position the surface in the center of the output and
   * compensate with black borders filling the rest of the output.
   */
  void XDGTopLevel::set_fullscreen(struct wl_client *client,
				   struct wl_resource *resource,
				   struct wl_resource *output){}
  /**
   */
  void XDGTopLevel::unset_fullscreen(struct wl_client *client,
				     struct wl_resource *resource){}
  /**
   * set the window as minimized
   *
   * Request that the compositor minimize your surface. There is no
   * way to know if the surface is currently minimized, nor is there
   * any way to unset minimization on this surface.
   *
   * If you are looking to throttle redrawing when minimized, please
   * instead use the wl_surface.frame event for this, as this will
   * also work with live previews on windows in Alt-Tab, Expose or
   * similar compositor features.
   */
  void XDGTopLevel::set_minimized(struct wl_client *client,
				  struct wl_resource *resource){}
}
