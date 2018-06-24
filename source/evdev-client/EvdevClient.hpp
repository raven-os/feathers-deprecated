#pragma once

#include <iostream>
#include <signal.h>
#include "XkbWrapper.hpp"

enum toDestroy
{
	ALL = 0,
	ONLY_CONTEXT
};

class EvdevClient
{
	int ret;
	int opt;
	struct keyboard *kbds;
	struct xkb_context *ctx;
	struct xkb_keymap *keymap;
	struct xkb_compose_table *compose_table;
	const char *rules;
	const char *model;
	const char *layout;
	const char *variant;
	const char *options;
	const char *keymap_path;
	const char *locale;
	struct sigaction act;
	bool valid;
	XkbWrapper xkbWrapper;
	enum toDestroy destructionFlag;

public:
	EvdevClient();
	~EvdevClient();

	bool initClient();
	void destroyClient();
	bool isValid() const;
};
