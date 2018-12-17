#pragma once

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>
#include "protocol/Input.hpp"

namespace protocol
{
  struct RepeatInfo{
    int32_t rate;
    int32_t delay;
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

  class FthKeyboard : public Input
  {
  public:
    FthKeyboard();

    bool handleKey(uint32_t time, uint32_t state);
    bool handleModifiers();
    void enter(struct wl_list *resources) override;
    void leave(struct wl_list *resources) override;

    uint32_t const getRate() const;
    uint32_t const getDelay() const;
    XkbHandler& getXkbHandler();

  private:
    // struct press press;
    // struct input_focus focus;
    // struct input_focus_handler focus_handler;
    XkbHandler xkb;
    struct wl_array keys;
	  struct wl_list handlers;
    struct wl_array client_keys;

    struct wl_list link;

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
