//#include <magma/VulkanHandler.hpp>$
#include "user-input/user-input.hpp"
#include "dummy.hpp"

int main () {
	struct wl_registry *registry = wl_display_get_registry(userInput::get().display);
	wl_registry_add_listener(registry, &(userInput::get().listeners->getListener("registry")), NULL);
	wl_display_roundtrip (userInput::get().display);

	userInput::get().egl_display = eglGetDisplay(userInput::get().display);
	eglInitialize(userInput::get().egl_display, NULL, NULL);

	userInput::get().xkb_context = xkb_context_new (XKB_CONTEXT_NO_FLAGS);

	userInput::createWindow();

	while (userInput::get().running) {
		wl_display_dispatch_pending(userInput::get().display);
		userInput::get().getWindow().draw();
	}

//	delete win;
	eglTerminate(userInput::get().egl_display);
	wl_display_disconnect(userInput::get().display);
	return 0;
}
