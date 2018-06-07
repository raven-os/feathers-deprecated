// class WindowListenerExample
// {
// public:
// };

//Pointer listener
template<class Listener>
int addListener(struct wl_pointer *wlPointer, Listener &listener) noexcept
{
  const wl_pointer_listener *pointerListener = new wl_pointer_listener
  {
    [](void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY) {
      return reinterpret_cast<Listener *>(data)->pointerEnter(pointer, serial, surface, surfX, surfY);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {
      return reinterpret_cast<Listener *>(data)->pointerLeave(pointer, serial, surface);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
      return reinterpret_cast<Listener *>(data)->pointerMotion(pointer, time, x, y);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
      return reinterpret_cast<Listener *>(data)->pointerButton(pointer, serial, time, button, state);
    },
    [](void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
      return reinterpret_cast<Listener *>(data)->pointerAxis(pointer, time, axis, value);
    },
    nullptr, nullptr, nullptr, nullptr
  };
  return wl_pointer_add_listener(wlPointer, pointerListener, reinterpret_cast<void *>(&listener));
}

//Keyboard listener
template<class Listener>
int addListener(struct wl_keyboard *wlKeyboard, Listener &listener) noexcept
{
  const wl_keyboard_listener *keyboardListener = new wl_keyboard_listener
  {
    [](void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
      return reinterpret_cast<Listener *>(data)->keyboardKeymap(keyboard, format, fd, size);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
      return reinterpret_cast<Listener *>(data)->keyboardEnter(keyboard, serial, surface, keys);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
      return reinterpret_cast<Listener *>(data)->keyboardLeave(keyboard, serial, surface);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
      return reinterpret_cast<Listener *>(data)->keyboardKey(keyboard, serial, time, key, state);
    },
    [](void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group) {
      return reinterpret_cast<Listener *>(data)->keyboardModifiers(keyboard, serial, modsDepressed, modsLatched, modsLocked, group);
    },
    nullptr
  };
  return wl_keyboard_add_listener(wlKeyboard, keyboardListener, reinterpret_cast<void *>(&listener));
}

//Seat listener
template<class Listener>
int addListener(struct wl_seat *wlSeat, Listener &listener)
{
  const wl_seat_listener *seatListener = new wl_seat_listener
  {
    [](void *data, struct wl_seat *seat, uint32_t capabilities) {
      return reinterpret_cast<Listener *>(data)->seatCapabilities(seat, capabilities);
    },
    [](void *data, struct wl_seat *seat, const char *name) {
      return reinterpret_cast<Listener *>(data)->seatName(seat, name);
    }
  };
  return  wl_seat_add_listener(wlSeat, seatListener, reinterpret_cast<void *>(&listener));
}

//Registry listener
template<class Listener>
int addListener(struct wl_registry *wlRegistry, Listener &listener) noexcept
{
  const wl_registry_listener *registryListener = new wl_registry_listener
    {
      [](void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	return reinterpret_cast<Listener *>(data)->registryAddObject(registry, name, interface, version);
      },
      [](void *data, struct wl_registry *registry, uint32_t name) {
	return reinterpret_cast<Listener *>(data)->registryRemoveObject(registry, name);
      }
    };
  return wl_registry_add_listener(wlRegistry, registryListener, reinterpret_cast<void *>(&listener));
}

//Window listener
template<class Listener>
int addListener(struct wl_shell_surface *wlShellSurface, Listener &listener) noexcept
{
  const wl_shell_surface_listener *shellSurfaceListener = new wl_shell_surface_listener
  {
    [](void *data, struct wl_shell_surface *shellSurface, uint32_t serial) {
      return reinterpret_cast<Listener *>(data)->shellSurfacePing(shellSurface, serial);
    },
    [](void *data, struct wl_shell_surface *shellSurface, uint32_t edges, int32_t width, int32_t height) {
      return reinterpret_cast<Listener *>(data)->shellSurfaceConfigure(shellSurface, edges, width, height);
    },
    [](void *data, struct wl_shell_surface *shellSurface) {
      return reinterpret_cast<Listener *>(data)->shellSurfacePopupDone(shellSurface);
    }
  };
  return wl_shell_surface_add_listener(wlShellSurface, shellSurfaceListener, reinterpret_cast<void *>(&listener));
}
