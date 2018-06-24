#include "XkbWrapper.hpp"

struct xkb_context *newContext() const
{
	struct xkb_context *ctx;

	ctx = xkb_context_new(0);
	if (!ctx)
	{
		return NULL;
	}
	xkb_context_include_path_append(ctx, DATA_PATH);

	return ctx;
}
