#pragma once

#include <iostream>
#include <signal.h>
#include <sys/epoll.h>
#include "FileDescriptorWrapper.hpp"
#include "XkbWrapper.hpp"

#define EVDEV_OFFSET 8

enum toDestroy
{
    ALL = 0,
    FROM_KEYBOARDS,
    FROM_KEYMAP,
    ONLY_CONTEXT,
    FROM_EPFD,
};

/* The meaning of the input_event 'value' field. */
enum keyState
{
    KEY_STATE_RELEASE = 0,
    KEY_STATE_PRESS = 1,
    KEY_STATE_REPEAT = 2,
};

class EvdevClient
{
    // ATTRIBUTES
    FileDescriptorWrapper fdWrapper;
    XkbWrapper xkbWrapper;
    struct keyboard *kbds;
    struct xkb_context *ctx;
    struct xkb_keymap *keymap;

    const char *rules;
    const char *model;
    const char *layout;
    const char *variant;
    const char *options;
    const char *keymap_path;
    const char *locale;

    bool valid;
    enum toDestroy destructionFlag;

    // FUNCTIONS
    void processEvent(struct keyboard *kbd, uint16_t type, uint16_t code, int32_t value);
    int readKeyboard(struct keyboard *kbd);

public:
    EvdevClient();
    ~EvdevClient();

    bool initClient();
    void destroyClient();
    bool isValid() const;
    void tick();
};
