#pragma once

#define DATA_PATH "./test/data"

struct XkbWrapper
{
	struct xkb_context *newContext() const;
};
