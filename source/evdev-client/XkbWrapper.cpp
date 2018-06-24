#include "XkbWrapper.hpp"

struct xkb_context *XkbWrapper::newContext() const
{
    struct xkb_context *ctx;
    enum xkb_context_flags flags(XKB_CONTEXT_NO_FLAGS);

    ctx = xkb_context_new(flags);
    xkb_context_include_path_append(ctx, DATA_PATH);
    return (ctx);
}

struct xkb_keymap *XkbWrapper::newKeymap(struct xkb_context *context) const
{
    struct xkb_keymap *keymap;
    enum xkb_keymap_compile_flags flags(XKB_KEYMAP_COMPILE_NO_FLAGS);

    keymap = xkb_keymap_new_from_names(context, NULL, flags);
    return (keymap);
}

/* Some heuristics to see if the device is a keyboard. */
bool XkbWrapper::isKeyboard(int fd) const
{
    unsigned long evbits[NLONGS(EV_CNT)] = { 0 };
    unsigned long keybits[NLONGS(KEY_CNT)] = { 0 };
    auto evdevBitIsSet(
        [](const unsigned long *array, int bit)
        {
            int index(bit / (CHAR_BIT * sizeof(long)));

            return (array[index] & (1LL << (bit % (CHAR_BIT * sizeof(long)))));
        }
    );

    errno = 0;
    ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits);
    if (errno)
    {
        return (false);
    }
    if (!evdevBitIsSet(evbits, EV_KEY))
    {
        return (false);
    }
    errno = 0;
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits);
    if (errno)
    {
        return (false);
    }
    for (int i = KEY_RESERVED; i <= KEY_MIN_INTERESTING; i++)
    {
        if (evdevBitIsSet(keybits, i))
        {
            return (true);
        }
    }
    return (false);
}

int XkbWrapper::keyboardNew(struct dirent *ent, struct xkb_keymap *keymap,
    struct keyboard **out) const
    {
        int ret;
        char *path;
        int fd;
        struct xkb_state *state;
        struct keyboard *kbd;

        ret = asprintf(&path, "/dev/input/%s", ent->d_name);
        if (ret < 0)
        {
            return (-ENOMEM);
        }
        fd = open(path, O_NONBLOCK | O_CLOEXEC | O_RDONLY);
        if (fd < 0)
        {
            free(path);
            return (-errno);
        }
        if (!isKeyboard(fd))
        {
            /* Dummy "skip this device" value. */
            close(fd);
            free(path);
            return (-ENOTSUP);
        }
        state = xkb_state_new(keymap);
        if (!state)
        {
            fprintf(stderr, "Couldn't create xkb state for %s\n", path);
            close(fd);
            free(path);
            return (-EFAULT);
        }
        kbd = (struct keyboard *)calloc(1, sizeof(*kbd));
        if (!kbd)
        {
            xkb_state_unref(state);
            close(fd);
            free(path);
            return (-ENOMEM);
        }
        kbd->path = path;
        kbd->fd = fd;
        kbd->state = state;
        kbd->compose_state = NULL;
        *out = kbd;
        return (0);
    }

    static int filterDeviceName(const struct dirent *ent)
    {
        return (!fnmatch("event*", ent->d_name, 0));
    }

    struct keyboard *XkbWrapper::getKeyboards(struct xkb_keymap *keymap) const
    {
        int ret;
        int i;
        int nents;
        struct dirent **ents;
        struct keyboard *kbds(NULL);
        struct keyboard *kbd(NULL);

        nents = scandir("/dev/input", &ents, filterDeviceName, alphasort);
        if (nents < 0)
        {
            fprintf(stderr, "Couldn't scan /dev/input: %s\n", strerror(errno));
            return (NULL);
        }
        for (i = 0; i < nents; i++)
        {
            ret = keyboardNew(ents[i], keymap, &kbd);
            if (ret)
            {
                if (ret == -EACCES)
                {
                    fprintf(stderr, "Couldn't open /dev/input/%s: %s. "
                    "You probably need root to run this.\n",
                    ents[i]->d_name, strerror(-ret));
                    break;
                }
                if (ret != -ENOTSUP)
                {
                    fprintf(stderr, "Couldn't open /dev/input/%s: %s. Skipping.\n",
                    ents[i]->d_name, strerror(-ret));
                }
                continue;
            }
            assert(kbd != NULL);
            {
                kbd->next = kbds;
                kbds = kbd;
            }
        }
        if (!kbds)
        {
            fprintf(stderr, "Couldn't find any useable keyboards.\n");
        }
        for (i = 0; i < nents; i++)
        {
            free(ents[i]);
        }
        free(ents);
        return (kbds);
    }

    void XkbWrapper::keyboardFree(struct keyboard *kbd) const
    {
        if (!kbd)
        {
            return;
        }
        if (kbd->fd >= 0)
        {
            close(kbd->fd);
        }
        free(kbd->path);
        xkb_state_unref(kbd->state);
        free(kbd);
    }

    void XkbWrapper::freeKeyboards(struct keyboard *kbds) const
    {
        struct keyboard *next;

        while (kbds)
        {
            next = kbds->next;
            keyboardFree(kbds);
            kbds = next;
        }
    }
