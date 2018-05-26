//#include <magma/VulkanHandler.hpp>$
#include <variant>
#include "user-input/UserInput.hpp"
#include "user-input/Listeners.hpp"
#include "user-input/WindowHandler.hpp"
#include "dummy.hpp"

int main () {
	struct wl_registry *registry = wl_display_get_registry(UserInput::get().display);
	wl_registry_add_listener(registry,
				 &std::get<struct wl_registry_listener>(
				   UserInput::get().listeners->getListener("registry")),
				 NULL);
	wl_display_roundtrip (UserInput::get().display);

	UserInput::get().egl_display = eglGetDisplay(UserInput::get().display);
	eglInitialize(UserInput::get().egl_display, NULL, NULL);

	UserInput::get().xkb_context = xkb_context_new (XKB_CONTEXT_NO_FLAGS);

	UserInput::createWindow();

	while (UserInput::get().running) {
		wl_display_dispatch_pending(UserInput::get().display);
		UserInput::get().getWindow().draw();
	}

//	delete win;
	eglTerminate(UserInput::get().egl_display);
	wl_display_disconnect(UserInput::get().display);
	return 0;
}
