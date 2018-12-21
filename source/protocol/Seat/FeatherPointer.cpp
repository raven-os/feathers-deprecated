#include <iostream>
#include "protocol/Seat/FeatherPointer.hpp"

namespace protocol {

  FthPointer::FthPointer() : Input()
  {
    // struct screen *screen = wl_container_of(swc.screens.next, screen, link);
    // struct swc_rectangle *geom = &screen->base.geometry;

    // /* Center cursor in the geometry of the first screen. */
    // screen = wl_container_of(swc.screens.next, screen, link);

    // center of the screen
    x = wl_fixed_from_int(1920 / 2);
    y = wl_fixed_from_int(1080 / 2);

    // pointer->focus_handler.enter = enter;
    // pointer->focus_handler.leave = leave;
    // pointer->client_handler.button = client_handle_button;
    // pointer->client_handler.axis = client_handle_axis;
    // pointer->client_handler.motion = client_handle_motion;
    wl_list_init(&handlers);
    // wl_list_insert(&pointer->handlers, &pointer->client_handler.link);
    wl_array_init(&buttons);

    // view_initialize(&pointer->cursor.view, &view_impl);
    // pointer->cursor.surface = NULL;
    // pointer->cursor.destroy_listener.notify = &handle_cursor_surface_destroy;
    // pointer->cursor.buffer = wld_create_buffer(swc.drm->context, swc.drm->cursor_w, swc.drm->cursor_h, WLD_FORMAT_ARGB8888, WLD_FLAG_MAP);
    // pointer->cursor.internal_buffer = NULL;

    // if (!pointer->cursor.buffer)
    //   return false;

    // pointer_set_cursor(pointer, cursor_left_ptr);

    // wl_list_for_each (screen, &swc.screens, link)
    //   view_attach(&screen->planes.cursor.view, pointer->cursor.buffer);

    // input_focus_initialize(&pointer->focus, &pointer->focus_handler);
    // pixman_region32_init(&pointer->region);
    std::cout << "New pointer" << std::endl;
  }

  void FthPointer::enter(struct wl_list *resources)
  {
    std::cout << "pointer enter" << std::endl;
  }

  void FthPointer::leave(struct wl_list *resources)
  {
    std::cout << "pointer leave" << std::endl;
  }

}
