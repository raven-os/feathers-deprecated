#pragma once

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>

namespace protocol
{
  struct RepeatInfo{
    int32_t rate;
    int32_t delay;
  };

  struct KeyboardHandler {
  	bool (*key)(struct keyboard *keyboard, uint32_t time, struct key *key, uint32_t state);
  	bool (*modifiers)(struct keyboard *keyboard, const struct keyboard_modifier_state *state);

  	struct wl_list link;
  };

  struct XkbHandler {
  	struct xkb_context *context;
  	struct xkb_state *state;

  	struct {
  		struct xkb_keymap *map;
  		int fd;
  		uint32_t size;
  		char *area;
  	} keymap;

  	struct {
  		uint32_t ctrl, alt, super, shift;
  	} indices;
  };

  struct FthKeyboard
  {
    // struct press press;
    // struct input_focus focus;
    // struct input_focus_handler focus_handler;
    XkbHandler xkb;
    struct wl_array keys;
	  struct wl_list handlers;
    struct wl_array client_keys;
	  KeyboardHandler client_handler;

    RepeatInfo repeatInfo;

	  struct {
      uint32_t depressed;
    	uint32_t latched;
    	uint32_t locked;
    	uint32_t group;
    } modifiers_states;

	  uint32_t modifiers;
  };
}
