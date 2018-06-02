class WindowListenerExample
{
public:
};

//Pointer listener
template<class Listener>
int addListener(struct wl_pointer *wlPointer, Listener &listener) noexcept
{
  const wl_pointer_listener *pointer_listener = new wl_pointer_listener
  {
    [](void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY) {
      return reinterpret_cast<Listener *>(data)->pointer_enter(pointer, serial, surface, surfX, surfY);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {
      return reinterpret_cast<Listener *>(data)->pointer_leave(pointer, serial, surface);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
      return reinterpret_cast<Listener *>(data)->pointer_motion(pointer, time, x, y);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
      return reinterpret_cast<Listener *>(data)->pointer_button(pointer, serial, time, button, state);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
      return reinterpret_cast<Listener *>(data)->pointer_axis(pointer, time, axis, value);
    },
    nullptr, nullptr, nullptr, nullptr
  };
  return wl_pointer_add_listener(wlPointer, pointer_listener, reinterpret_cast<void *>(&listener));
}

//Keyboard listener
template<class Listener>
int addListener(struct wl_keyboard *wlKeyboard, Listener &listener) noexcept
{
  const wl_keyboard_listener *keyboard_listener = new wl_keyboard_listener
  {
    [](void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
      return reinterpret_cast<Listener *>(data)->keyboard_keymap(keyboard, format, fd, size);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
      return reinterpret_cast<Listener *>(data)->keyboard_enter(keyboard, serial, surface, keys);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
      return reinterpret_cast<Listener *>(data)->keyboard_leave(keyboard, serial, surface);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
      return reinterpret_cast<Listener *>(data)->keyboard_key(keyboard, serial, time, key, state);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
      return reinterpret_cast<Listener *>(data)->keyboard_modifiers(keyboard, serial, mods_depressed, mods_latched, mods_locked, group);
    },
    nullptr
  };
  return wl_keyboard_add_listener(wlKeyboard, keyboard_listener, reinterpret_cast<void *>(&listener));
}

//Seat listener
template<class Listener>
int addListener(struct wl_seat *wlSeat, Listener &listener)
{
  const wl_seat_listener *seat_listener = new wl_seat_listener
  {
    [](void *data, struct wl_seat *seat, uint32_t capabilities) {
      return reinterpret_cast<Listener *>(data)->seat_capabilities(seat, capabilities);
    },
    [](void *data, struct wl_seat *seat, const char *name) {
      return reinterpret_cast<Listener *>(data)->seat_name(seat, name);
    }
  };
  return  wl_seat_add_listener(wlSeat, seat_listener, reinterpret_cast<void *>(&listener));
}

//Registry listener
template<class Listener>
int addListener(struct wl_registry *wlRegistry, Listener &listener) noexcept
{
  const wl_registry_listener *registry_listener = new wl_registry_listener
    {
      [](void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	return reinterpret_cast<Listener *>(data)->registry_add_object(registry, name, interface, version);
      },
      [](void *data, struct wl_registry *registry, uint32_t name) {
	return reinterpret_cast<Listener *>(data)->registry_remove_object(registry, name);
      }
    };
  return wl_registry_add_listener(wlRegistry, registry_listener, reinterpret_cast<void *>(&listener));
}

//Window listener
template<class Listener>
int addListener(struct wl_shell_surface *wlShellSurface, Listener &listener) noexcept
{
  const wl_shell_surface_listener *shell_surface_listener = new wl_shell_surface_listener
  {
    [](void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
      return reinterpret_cast<Listener *>(data)->shell_surface_ping(shell_surface, serial);
    },
    [](void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
      return reinterpret_cast<Listener *>(data)->shell_surface_configure(shell_surface, edges, width, height);
    },
    [](void *data, struct wl_shell_surface *shell_surface) {
      return reinterpret_cast<Listener *>(data)->shell_surface_popup_done(shell_surface);
    }
  };
  return wl_shell_surface_add_listener(wlShellSurface, shell_surface_listener, reinterpret_cast<void *>(&listener));
}
