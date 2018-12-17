#include "protocol/Input.hpp"

namespace protocol {

  Input::Input()
  {
    client = NULL;
    //view = NULL;
    //view_destroy_listener.notify = &handle_focus_view_destroy;

    wl_list_init(&active);
    wl_list_init(&inactive);
    wl_signal_init(&event_signal);
  }

  Input::~Input()
  {
    //TODO destroy resources ?
  }

  void Input::focusAddResource(struct wl_resource *resource)
  {
    wl_list resources;
    wl_list *target = &inactive;

  	wl_list_init(&resources);
  	wl_list_insert(&resources, wl_resource_get_link(resource));

  	/* If this new input resource corresponds to the focused client, send an enter event. */
  	if (wl_resource_get_client(resource) == client) {
  		enter(&resources);
  		target = &active;
	   }

	   wl_list_insert_list(target, &resources);
  }

  void Input::focusRemoveResource(struct wl_resource *resource)
  {
    wl_list_remove(wl_resource_get_link(resource));
  }

  void Input::focusSet()
  {

  }

}
