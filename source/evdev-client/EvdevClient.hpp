#pragma once

#include <iostream>
#include <signal.h>
#include <sys/epoll.h>
#include "XkbWrapper.hpp"

#define EVDEV_OFFSET 8

enum toDestroy
{
    ALL = 0,
    FROM_KEYBOARDS,
    FROM_KEYMAP,
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
    bool terminate;
    bool valid;
    XkbWrapper xkbWrapper;
    enum toDestroy destructionFlag;

public:
    EvdevClient();
    ~EvdevClient();

    bool initClient();
    void destroyClient();
    bool isValid() const;
    void process_event(struct keyboard *kbd, uint16_t type, uint16_t code, int32_t value);
    int read_keyboard(struct keyboard *kbd);
    int loop();
};
