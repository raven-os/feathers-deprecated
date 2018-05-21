#include "user-input/registryListener.hpp"

registryListener::registryListener(std::map<std::string, var_listener> &listeners) {
  static struct wl_registry_listener registry_listener =
          {&registryListener::egistry_add_object,
           &registryListener::registry_remove_object};
  listeners.emplace("registry", registry_listener);
}

void registryListener::registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	if (!strcmp(interface,"wl_compositor")) {
		userInput::get().compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
	}
	else if (!strcmp(interface,"wl_shell")) {
		userInput::get().shell = static_cast<wl_shell *>(wl_registry_bind (registry, name, &wl_shell_interface, 1));
	}
	else if (!strcmp(interface,"wl_seat")) {
		userInput::get().seat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
		wl_seat_add_listener(userInput::get().seat, &seat_listener, NULL);
	}
}

void registryListener::egistry_remove_object (void *data, struct wl_registry *registry, uint32_t name) {

}
