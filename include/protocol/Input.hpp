#pragma once

#include <wayland-server.h>

namespace protocol
{
  class Input
  {
  public:
    Input();
    Input(Input const &) = delete;
    Input(Input &&) = delete;

    virtual ~Input();

    virtual void enter(struct wl_list *resources) = 0;
    virtual void leave(struct wl_list *resources) = 0;

    void focusAddResource(struct wl_resource *resource);
    void focusRemoveResource(struct wl_resource *resource);
    void focusSet(/*struct compositor_view *view*/);

  protected:
    struct {
      uint32_t value;
      uint32_t serial;
      void *data;
    } press;

    wl_client *client;
    //struct compositor_view *view;
    wl_listener view_destroy_listener;

    wl_list active;
    wl_list inactive;

    wl_signal event_signal;
  };
}
