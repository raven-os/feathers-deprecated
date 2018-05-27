#include "user-input/RegistryListener.hpp"

RegistryListener::RegistryListener(std::map<std::string, Listener> &listeners) {
  static struct wl_registry_listener registry_listener =
          {&RegistryListener::registry_add_object,
           &RegistryListener::registry_remove_object};
  listeners.emplace("registry", registry_listener);
}

void RegistryListener::registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	if (!strcmp(interface,"wl_compositor")) {
		UserInput::get().compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
	}
	else if (!strcmp(interface,"wl_shell")) {
		UserInput::get().shell = static_cast<wl_shell *>(wl_registry_bind (registry, name, &wl_shell_interface, 1));
	}
	else if (!strcmp(interface,"wl_seat")) {
		UserInput::get().seat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
		wl_seat_add_listener(UserInput::get().seat,
				     &std::get<struct wl_seat_listener>(
				       UserInput::get().listeners->getListener("seat")),
				     NULL);
	}
}

void RegistryListener::registry_remove_object (void *data, struct wl_registry *registry, uint32_t name) {

}
