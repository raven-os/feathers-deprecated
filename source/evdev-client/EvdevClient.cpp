#include "EvdevClient.hpp"

EvdevClient::EvdevClient()
:
compose_table(NULL),
rules(NULL),
model(NULL),
layout(NULL),
variant(NULL),
options(NULL),
keymap_path(NULL),
valid(false)
{
	std::cout << "EvdevClient constructed." << std::endl;
}

EvdevClient::~EvdevClient()
{
	destroyClient();
	std::cout << "EvdevClient destructed." << std::endl;
}

bool EvdevClient::initClient()
{
	ctx = xkbWrapper.newContext();
	if (!ctx)
	{
		fprintf(stderr, "Couldn't create xkb context.\n");
		return (valid);
	}
	destructionFlag = toDestroy::ONLY_CONTEXT;
	keymap = xkbWrapper.newKeymap(ctx);
	if (!keymap)
	{
		fprintf(stderr, "Couldn't create xkb keymap.\n");
		return (valid);
	}
	destructionFlag = toDestroy::FROM_KEYMAP;
	kbds = xkbWrapper.getKeyboards(keymap);
	if (!kbds) {
		fprintf(stderr, "Couldn't get any keyboard.\n");
		return (valid);
	}
	destructionFlag = toDestroy::FROM_KEYBOARDS;
	valid = true;
	return (valid);
}

void EvdevClient::destroyClient()
{
	switch (destructionFlag)
	{
		case toDestroy::ALL:
		case toDestroy::FROM_KEYBOARDS:
		xkbWrapper.freeKeyboards(kbds);
		[[fallthrough]];
		case toDestroy::FROM_KEYMAP:
		xkb_keymap_unref(keymap);
		[[fallthrough]];
		case toDestroy::ONLY_CONTEXT:
		xkb_context_unref(ctx);
	}
}

bool EvdevClient::isValid() const
{
	return (valid);
}
