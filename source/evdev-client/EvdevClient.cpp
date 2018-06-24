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
	xkb_context_unref(ctx);
	std::cout << "EvdevClient destructed." << std::endl;
}

bool EvdevClient::initClient()
{
	ctx = xkbWrapper.newContext();
	if (!ctx)
	{
			fprintf(stderr, "Couldn't create xkb context\n");
			return (valid);
	}
	valid = true;
	return (valid);
}

bool EvdevClient::isValid() const
{
	return (valid);
}
