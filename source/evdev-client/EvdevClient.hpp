#pragma once

#include <iostream>
#include <signal.h>
#include "XkbWrapper.hpp"

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

public:
	EvdevClient();
	~EvdevClient();

	bool initClient();
	bool isValid() const;
};
