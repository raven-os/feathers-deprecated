#pragma once

#include <wayland-server.h>

namespace protocol
{
  class Seat
  {
  public:
    Seat() = delete;

    void get_pointer(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void get_keyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void get_touch(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void release(struct wl_client *client, struct wl_resource *resource);
  };
}
