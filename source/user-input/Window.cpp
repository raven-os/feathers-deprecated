#include "user-input/WindowHandler.hpp"

WindowHandler::WindowHandler(EGLDisplay &egl_display, struct wl_shell *shell,
			     struct wl_compositor *compositor) :
  egl_display(egl_display),
  shell(shell)
{
	EGLint attributes[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_NONE};
	EGLConfig config;
	EGLint num_config;

	eglBindAPI(EGL_OPENGL_API);
	eglChooseConfig (egl_display, attributes, &config, 1, &num_config);

	window.egl_context = eglCreateContext (egl_display, config, EGL_NO_CONTEXT, NULL);
  window.surface = wl_compositor_create_surface (compositor);
	window.shell_surface = wl_shell_get_shell_surface (shell, window.surface);
	wl_shell_surface_add_listener (window.shell_surface,
																&std::get<struct wl_shell_surface_listener>(
																 UserInput::get().listeners->getListener("shell")),
																 &window);
	wl_shell_surface_set_toplevel (window.shell_surface);
	window.egl_window = wl_egl_window_create (window.surface, WIDTH, HEIGHT);
	window.egl_surface = eglCreateWindowSurface (egl_display, config, window.egl_window, NULL);
	eglMakeCurrent (egl_display, window.egl_surface, window.egl_surface, window.egl_context);
}

WindowHandler::~WindowHandler(){
  eglDestroySurface(egl_display, window.egl_surface);
  wl_egl_window_destroy(window.egl_window);
  wl_shell_surface_destroy(window.shell_surface);
  wl_surface_destroy(window.surface);
  eglDestroyContext(egl_display, window.egl_context);
}

void WindowHandler::draw() {
  glClearColor(0.0, 1.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(egl_display, window.egl_surface);
}

/*struct window const& WindowHandler::getWindow() const {
  return window;
}*/
