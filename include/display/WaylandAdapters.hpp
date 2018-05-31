class WindowListenerExample
{
public:
};

template<class Listener>
void addListener(struct wl_shell_surface *wlShellSurface, Listener &listener)
{
  const struct wl_shell_surface_listener shell_surface_listener =
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
  wl_shell_surface_add_listener(wlShellSurface, &shell_surface_listener, reinterpret_cast<void *>(&listener));
}



template<class Listener>
void addListener(struct wl_registry *wlRegistry, Listener &listener)
{
  const struct wl_registry_listener registry_listener =
    {
      [](void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	return reinterpret_cast<Listener *>(data)->registry_add_object(registry, name, interface, version);
      },
      [](void *data, struct wl_registry *registry, uint32_t name) {
	return reinterpret_cast<Listener *>(data)->registry_remove_object(registry, name);
      }
    };
  wl_registry_add_listener(wlRegistry, &registry_listener, reinterpret_cast<void *>(&listener));
}

