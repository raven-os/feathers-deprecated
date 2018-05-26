#include "user-input/Listeners.hpp"

Listeners::Listeners() {
  const PointerListener pL = PointerListener(listeners);
  const KeyboardListener kL = KeyboardListener(listeners);
  const SeatListener sL = SeatListener(listeners);
  const RegistryListener rL = RegistryListener(listeners);
  const WindowListener wL = WindowListener(listeners);
}

Listeners::~Listeners() {

}

varListener const &Listeners::getListener(std::string key) const {
  return listeners.at(key);
}
