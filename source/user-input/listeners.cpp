#include "user-input/listeners.hpp"

Listeners::Listeners() {
  const pointerListener pL = pointerListener(listeners);
  //const keyboardListener kL = keyboardListener(listeners);
  //const seatListener sL = seatListener(listeners);
  //const registryListener rL = registryListener(listeners);
  const windowListener wL = windowListener(listeners);
}

Listeners::~Listeners() {

}

var_listener const &Listeners::getListener(std::string key) const {
  return listeners.at(key);
}
