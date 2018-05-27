// g++ -o user-input ../main.cpp UserInput.cpp Window.cpp Listeners.cpp listeners/*.cpp -lwayland-client -lwayland-egl -lEGL -lGL -lxkbcommon -I../../include/ -g -std=c++17

#include "user-input/UserInput.hpp"
#include "user-input/Listeners.hpp"
#include "user-input/WindowHandler.hpp"

UserInput UserInput::instance = UserInput();

UserInput::UserInput() {
	UserInput::instance.compositor = NULL;
	UserInput::instance.shell = NULL;
	UserInput::instance.seat = NULL;
	UserInput::instance.keymap = NULL;
	UserInput::instance.xkb_state = NULL;
	UserInput::instance.running = true;
	UserInput::instance.display = wl_display_connect(NULL);
	listeners = new Listeners();
}

UserInput &UserInput::get() {
	return instance;
}

WindowHandler &UserInput::getWindow() {
	return *win;
}

void UserInput::createWindow() {
	UserInput::instance.win = new WindowHandler(UserInput::instance.egl_display,
																							UserInput::instance.shell,
																							UserInput::instance.compositor);
}
