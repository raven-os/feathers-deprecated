#pragma once

#include <wayland-server.h>

namespace protocol
{
  struct FthSeatClient {
    wl_client *client;
    struct FthSeat *seat;

    // lists of wl_resource
    // struct wl_list resources;
    // struct wl_list pointers;
    // struct wl_list keyboards;
    // struct wl_list touches;
    // struct wl_list data_devices;
    // struct wl_list primary_selection_devices;

    struct {
      wl_signal destroy;
    } events;

    wl_list link;
  };

  struct FthSeatPointerState
  {
    struct FthSeat *seat;
  	// struct wlr_seat_client *focused_client;
  	// struct wlr_surface *focused_surface;

  	// struct wlr_seat_pointer_grab *grab;
  	// struct wlr_seat_pointer_grab *default_grab;

  	uint32_t button_count;
  	uint32_t grab_button;
  	uint32_t grab_serial;
  	uint32_t grab_time;

  	wl_listener surface_destroy;

  	struct {
  		wl_signal focus_change; // wlr_seat_pointer_focus_change_event
  	} events;
  };

  struct FthSeatTouchState
  {
  };

  struct FthSeatKeyboardState
  {
  };

  struct FthSeat
  {
    wl_global *global;
    wl_display *display;
    wl_list clients;

    char *seat_name;
    uint32_t capabilities;

    FthSeatPointerState pointer_state;
	  FthSeatKeyboardState keyboard_state;
	  FthSeatTouchState touch_state;

    struct {
  		wl_signal pointer_grab_begin;
  		wl_signal pointer_grab_end;

  		wl_signal keyboard_grab_begin;
  		wl_signal keyboard_grab_end;

  		wl_signal touch_grab_begin;
  		wl_signal touch_grab_end;

  		wl_signal request_set_cursor;

  		wl_signal selection;
  		wl_signal primary_selection;

  		wl_signal start_drag;
  		wl_signal new_drag_icon;

  		wl_signal destroy;
  	} events;
  };

}
