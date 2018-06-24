#include "XkbWrapper.hpp"

struct xkb_context *XkbWrapper::newContext() const
{
	struct xkb_context *ctx;
	enum xkb_context_flags flags(XKB_CONTEXT_NO_FLAGS);

	ctx = xkb_context_new(flags);
	xkb_context_include_path_append(ctx, DATA_PATH);
	return (ctx);
}

struct xkb_keymap *XkbWrapper::newKeymap(struct xkb_context *context) const
{
	struct xkb_keymap *keymap;
	enum xkb_keymap_compile_flags flags(XKB_KEYMAP_COMPILE_NO_FLAGS);

	keymap = xkb_keymap_new_from_names(context, NULL, flags);
	return (keymap);
}
