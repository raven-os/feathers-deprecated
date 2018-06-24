#pragma once

#include <iostream>
#include <signal.h>

class EvdevClient
{
	int ret;
	int opt;
	struct keyboard *kbds;
	struct xkb_context *ctx;
	struct xkb_keymap *keymap;
	struct xkb_compose_table *compose_table = NULL;
	const char *rules = NULL;
	const char *model = NULL;
	const char *layout = NULL;
	const char *variant = NULL;
	const char *options = NULL;
	const char *keymap_path = NULL;
	const char *locale;
	struct sigaction act;

public:
	EvdevClient();
	~EvdevClient();
};
