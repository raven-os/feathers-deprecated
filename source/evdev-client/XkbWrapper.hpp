#pragma once

#include "xkbcommon/xkbcommon.h"

#define DATA_PATH "./test/data"

struct XkbWrapper
{
	struct xkb_context *newContext() const;
	struct xkb_keymap *newKeymap(struct xkb_context *context) const;
};
