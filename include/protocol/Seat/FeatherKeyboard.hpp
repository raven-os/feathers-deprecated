#pragma once

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>
#include "protocol/Input.hpp"

namespace protocol
{
  struct RepeatInfo{
    const int32_t rate = 40;
    const int32_t delay = 500;
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
  		uint32_t ctrl;
      uint32_t alt;
      uint32_t super;
      uint32_t shift;
  	} indices;
  };

  class FthKeyboard : public Input
  {
  public:
    FthKeyboard();
    ~FthKeyboard();

    void enter(struct wl_list *resources) override;
    void leave(struct wl_list *resources) override;

    void handleKeymap();
    bool handleKey(uint32_t time, uint32_t state);
    bool handleModifiers();
    void handleRepeatInfo();

    bool reset();

    uint32_t getRate() const;
    uint32_t getDelay() const;
    XkbHandler& getXkbHandler();

  private:
    bool updateKeymap();

  private:
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

    const char *KEYMAP_FILE_TEMPLATE = "swc-xkb-keymap-XXXXXX";
  };
}
