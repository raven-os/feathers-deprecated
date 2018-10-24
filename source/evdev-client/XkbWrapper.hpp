#pragma once

#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <climits>
#include <linux/input.h>
#include <assert.h>
#include <xkbcommon/xkbcommon.h>
#include <iostream>

#define DATA_PATH "./test/data"
#define NLONGS(n) (((n) + (CHAR_BIT * sizeof(long)) - 1) / (CHAR_BIT * sizeof(long)))

struct keyboard
{
    char *path;
    int fd;
    struct xkb_state *state;
    struct xkb_compose_state *compose_state;
    struct keyboard *next;
};

struct XkbWrapper
{
    ~XkbWrapper();
    struct xkb_context *newContext() const;
    struct xkb_keymap *newKeymap(struct xkb_context *context) const;
    struct keyboard *getKeyboards(struct xkb_keymap *keymap) const;
    void freeKeyboards(struct keyboard *kbds) const;

private:
    bool isKeyboard(int fd) const;
    int keyboardNew(std::string entName,
        struct xkb_keymap *keymap,
        struct keyboard **out
    ) const;
    void freeKeyboard(struct keyboard *kbd) const;
};
