#pragma once

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>

namespace protocol
{
  struct RepeatInfo{
		int32_t rate;
		int32_t delay;
	};

  struct FthKeyboard
  {
    char *keymap_string;
	  size_t keymap_size;
	  struct xkb_keymap *keymap;
	  struct xkb_state *xkb_state;
    RepeatInfo repeat_info;

    struct {
		    struct wl_signal key;

		    struct wl_signal modifiers;
		    struct wl_signal keymap;
		    struct wl_signal repeat_info;
	  } events;

	  void *data;
  };
}
