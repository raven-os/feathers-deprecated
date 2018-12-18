#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "protocol/Seat/FeatherKeyboard.hpp"

namespace protocol {

  FthKeyboard::FthKeyboard() : Input()
  {
    if (!(xkb.context = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
  		std::cerr << "Could not create XKB context" << std::endl;
      return ; //TODO throw exeception
  	}

  	if (!(xkb.keymap.map = xkb_keymap_new_from_names(xkb.context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS))) {
  		std::cerr << "Could not create XKB keymap" << std::endl;
      xkb_context_unref(xkb.context);
      return ;
  	}

  	if (!(xkb.state = xkb_state_new(xkb.keymap.map))) {
  		std::cerr << "Could not create XKB state" << std::endl;
      xkb_context_unref(xkb.context);
  		xkb_keymap_unref(xkb.keymap.map);
      return ;
  	}

  	if (!updateKeymap()) {
  		std::cerr << "Could not update XKB keymap" << std::endl;
      xkb_context_unref(xkb.context);
      xkb_keymap_unref(xkb.keymap.map);
  		xkb_state_unref(xkb.state);
      return ;
  	}

    modifiers_states = {0};
    modifs = 0;
    wl_array_init(&client_keys);
    wl_array_init(&keys);
    wl_list_init(&handlers);
    wl_list_insert(&handlers, &link);
    std::cout << "Keyboard bind !" << std::endl;
  }

  FthKeyboard::~FthKeyboard()
  {
    wl_array_release(&client_keys);
  	wl_array_release(&keys);
  	munmap(xkb.keymap.area, xkb.keymap.size);
  	close(xkb.keymap.fd);
  	xkb_state_unref(xkb.state);
  	xkb_keymap_unref(xkb.keymap.map);
  	xkb_context_unref(xkb.context);
  }

  bool FthKeyboard::key(uint32_t time, uint32_t state)
  {
    return true;
  }

  bool FthKeyboard::modifiers()
  {
    return true;
  }

  void FthKeyboard::enter(struct wl_list *resources)
  {

  }

  void FthKeyboard::leave(struct wl_list *resources)
  {

  }

  bool FthKeyboard::updateKeymap() {
    const char *keymap_directory;

    if (!(keymap_directory = getenv("XDG_RUNTIME_DIR")))
      keymap_directory = "/tmp";

    xkb.indices.ctrl = xkb_keymap_mod_get_index(xkb.keymap.map, XKB_MOD_NAME_CTRL);
    xkb.indices.alt = xkb_keymap_mod_get_index(xkb.keymap.map, XKB_MOD_NAME_ALT);
    xkb.indices.super = xkb_keymap_mod_get_index(xkb.keymap.map, XKB_MOD_NAME_LOGO);
    xkb.indices.shift = xkb_keymap_mod_get_index(xkb.keymap.map, XKB_MOD_NAME_SHIFT);

    /* In order to send the keymap to clients, we must first convert it to a
     * string and then mmap it to a file. */
    std::string keymap_string = std::string(xkb_keymap_get_as_string(xkb.keymap.map, XKB_KEYMAP_FORMAT_TEXT_V1));

    if (keymap_string.empty()) {
      std::cerr << "Could not get XKB keymap as a string" << std::endl;
      return false;
    }

    std::string keymap_path = std::string(keymap_directory) + "\n" + KEYMAP_FILE_TEMPLATE;

    xkb.keymap.size = static_cast<uint32_t>(keymap_string.size() + 1);
    xkb.keymap.fd = mkostemp(const_cast<char *>(keymap_path.c_str()), O_CLOEXEC);

    if (xkb.keymap.fd == -1) {
      std::cerr << "Could not create XKB keymap file" << std::endl;
      return false;
    }

    unlink(keymap_path.c_str());

    if (posix_fallocate(xkb.keymap.fd, 0, xkb.keymap.size) != 0) {
      std::cerr << "Could not resize XKB keymap file" << std::endl;
      close(xkb.keymap.fd);
      return false;
    }

    xkb.keymap.area = static_cast<char *>(mmap(NULL, xkb.keymap.size, PROT_READ | PROT_WRITE, MAP_SHARED, xkb.keymap.fd, 0));

    if (xkb.keymap.area == MAP_FAILED) {
      std::cerr << "Could not mmap XKB keymap string" << std::endl;
      close(xkb.keymap.fd);
      return false;
    }

    strcpy(xkb.keymap.area, keymap_string.c_str());
    return true;
  }

  uint32_t FthKeyboard::getRate() const
  {
    return repeatInfo.rate;
  }

  uint32_t FthKeyboard::getDelay() const
  {
    return repeatInfo.delay;
  }

  XkbHandler& FthKeyboard::getXkbHandler()
  {
    return xkb;
  }

}
