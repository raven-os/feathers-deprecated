#pragma once

#include <map>
#include "user-input/UserInput.hpp"
#include "user-input/Listeners.hpp"

class RegistryListener {
  public:
    RegistryListener(std::map<std::string, varListener> &listeners);
    ~RegistryListener() = default;

  private:
    static void registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void registry_remove_object (void *data, struct wl_registry *registry, uint32_t name);

};
