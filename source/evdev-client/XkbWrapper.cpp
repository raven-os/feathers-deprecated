#include "XkbWrapper.hpp"

struct xkb_context *XkbWrapper::newContext() const
{
	struct xkb_context *ctx;
	enum xkb_context_flags flags(XKB_CONTEXT_NO_FLAGS);

	ctx = xkb_context_new(flags);
	if (!ctx)
	{
		return NULL;
	}
	xkb_context_include_path_append(ctx, DATA_PATH);

	return ctx;
}
