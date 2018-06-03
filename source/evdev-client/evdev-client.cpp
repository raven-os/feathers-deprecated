#include <xkbcommon/xkbcommon.h>
#include <iostream>

int main()
{
  struct xkb_context *ctx;
  ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!ctx)
    {
      std::cout << "ERROR 1" << std::endl;
      return 1;
    }
  struct xkb_keymap *keymap;
  /* Example RMLVO for Icelandic Dvorak. */
  struct xkb_rule_names names = {
    .rules = NULL,
    .model = "",
    .layout = "",
    .variant = "",
    .options = ""
  };
  keymap = xkb_keymap_new_from_names(ctx, &names,
				     XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!keymap)
    {
      std::cout << "ERROR 2" << std::endl;
      return 1;
    }
  struct xkb_state *state;
  state = xkb_state_new(keymap);
  if (!state)
    {
      std::cout << "ERROR 3" << std::endl;
      return 1;
    }
  //<key event structure> event;
  xkb_keycode_t keycode;
  xkb_keysym_t keysym;
  //keycode = event->keycode;
  keysym = xkb_state_key_get_one_sym(state, keycode);
  char keysym_name[64];
  if (keysym == XKB_KEY_space)
    {
      xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
      std::cout << keysym_name << std::endl;
    }

  xkb_state_unref(state);
  xkb_keymap_unref(keymap);
  xkb_context_unref(ctx);
  std::cout << "NO ERROR" << std::endl;
  return 0;
}
