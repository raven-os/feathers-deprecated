#include "display/WaylandSurface.hpp"

namespace display
{
  WaylandSurface::WaylandSurface()
    : wlDisplay(wl_display_connect(NULL)), xkb_context(xkb_context_new (XKB_CONTEXT_NO_FLAGS))
  {
    if (!wlDisplay)
      throw std::runtime_error("Could not connect to display");
    if (!(wlRegistry = wl_display_get_registry(wlDisplay)))
      throw std::runtime_error("Could not get registry");
    if (int err = addListener(wlRegistry, *this))
      {
	std::cerr << "error: " << err << std::endl;
	throw std::runtime_error("Received error");
      }
    wl_display_roundtrip(wlDisplay);
    if (!wlCompositor) {
      throw std::runtime_error("Could not find compositor");
    }
    if (!wlShell) {
      throw std::runtime_error("Could not find shell");
    }
    if (!wlSeat) {
      throw std::runtime_error("Could not find seat");
    }
    wlShellSurface = wl_shell_get_shell_surface(wlShell, wlSurface);
    if (!wlShellSurface) {
      throw std::runtime_error("Could not get shell surface");
    }
    addListener(wlShellSurface, *this);
    wl_shell_surface_set_toplevel(wlShellSurface);
    wl_display_roundtrip(wlDisplay);
  }

  void WaylandSurface::pointer_enter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
  {
    printf ("pointer enter\n");
  }

  void WaylandSurface::pointer_leave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
  {
    printf ("pointer leave\n");
  }

  void WaylandSurface::pointer_motion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
  {
    printf ("pointer motion %f %f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
  }

  void WaylandSurface::pointer_button(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
  {
    printf ("pointer button (button %d, state %d)\n", button, state);
  }

  void WaylandSurface::pointer_axis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
  {
	   printf ("pointer axis\n");
  }

  void WaylandSurface::keyboard_keymap(struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size)
  {
    char *keymap_string = static_cast<char *>(mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));

  	xkb_keymap_unref(keymap);
  	keymap = xkb_keymap_new_from_string(xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
  	munmap (keymap_string, size);
  	close(fd);
  	xkb_state_unref(xkb_state);
  	xkb_state = xkb_state_new(keymap);
  }

  void WaylandSurface::keyboard_enter(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
  {
    printf("Entering the window\n");
  }

  void WaylandSurface::keyboard_leave(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
  {
    printf("Leaving the window\n");
  }

  void WaylandSurface::keyboard_key (struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
  {
  	if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
  		xkb_keysym_t keysym = xkb_state_key_get_one_sym(xkb_state, key + 8);
  		uint32_t utf32 = xkb_keysym_to_utf32(keysym);
  		if (utf32) {
  			if (utf32 >= 0x21 && utf32 <= 0x7E) {
  				printf ("the key %c was pressed\n", (char)utf32);
  				if (utf32 == 'q')
            running = false;
  			}
  			else {
  				printf ("the key U+%04X was pressed\n", utf32);
  			}
  		}
  		else {
  			char name[64];
  			xkb_keysym_get_name(keysym, name, 64);
  			printf ("the key %s was pressed\n", name);
  		}
  	}
  }

  void WaylandSurface::keyboard_modifiers (struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
  {
  	xkb_state_update_mask(xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
  }

  void WaylandSurface::seat_capabilities(struct wl_seat *seat, uint32_t capabilities) {
  	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
  		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
  		addListener(pointer, *this);
  	}
  	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
  		struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
  		addListener(keyboard, *this);
  	}
  }

  void WaylandSurface::seat_name(struct wl_seat *seat, const char *name)
  {

  }


  void WaylandSurface::shell_surface_ping (struct wl_shell_surface *shell_surface, uint32_t serial)
  {
    wl_shell_surface_pong (shell_surface, serial);
  }

  void WaylandSurface::shell_surface_configure(struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
  {
    std::cout << "shell_surface_configure" << std::endl;
  }

  void WaylandSurface::shell_surface_popup_done(struct wl_shell_surface *shell_surface)
  {
  }

  void WaylandSurface::registry_add_object(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
  {
    if (!strcmp(interface,"wl_compositor"))
      {
      	wlCompositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
      	wlSurface = wl_compositor_create_surface(wlCompositor);
      	if (!wlSurface) {
      	  throw std::runtime_error("Could not create surface");
      	}
      }
    else if (!strcmp(interface,"wl_shell"))
      {
	      wlShell = static_cast<wl_shell *>(wl_registry_bind(registry, name, &wl_shell_interface, 1));
      }
    else if (!strcmp(interface,"wl_seat"))
      {
    	   wlSeat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
         addListener(wlSeat, *this);
      }
  }

  void WaylandSurface::registry_remove_object(struct wl_registry *registry, uint32_t name) {
  }

  void WaylandSurface::dispatch() {
    wl_display_dispatch_pending(wlDisplay);
  }

  bool WaylandSurface::isRunning() const
  {
    return running;
  }

}
