#include "XkbWrapper.hpp"

XkbWrapper::~XkbWrapper()
{
    //freeKeyboards();
}

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
        ret = keyboardNew(ents[i]->d_name, keymap, &kbd);
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
        }
        else if (kbd != NULL)
        {
            kbd->next = kbds;
            kbds = kbd;
        }
        else
        {
            std::bad_alloc e;
            throw e;
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

int XkbWrapper::keyboardNew(std::string entName,
    struct xkb_keymap *keymap,
    struct keyboard **out
) const
{
    std::string spath("/dev/input/" + entName);
    int fd;
    struct xkb_state *state;
    struct keyboard *kbd;

    fd = open(spath.c_str(), O_NONBLOCK | O_CLOEXEC | O_RDONLY);
    if (fd < 0)
    {
        return (-errno);
    }
    if (!isKeyboard(fd))
    {
        /* Dummy "skip this device" value. */
        close(fd);
        return (-ENOTSUP);
    }
    state = xkb_state_new(keymap);
    if (!state)
    {
        fprintf(stderr, "Couldn't create xkb state for %s\n", spath.c_str());
        close(fd);
        return (-EFAULT);
    }
    kbd = new struct keyboard[1];
    if (!kbd)
    {
        xkb_state_unref(state);
        close(fd);
        return (-ENOMEM);
    }
    kbd->path = strcpy(new char[spath.length() + 1], spath.c_str());
    kbd->fd = fd;
    kbd->state = state;
    kbd->compose_state = NULL;
    *out = kbd;
    return (0);
}

void XkbWrapper::freeKeyboard(struct keyboard *kbd) const
{
    if (!kbd)
    {
        return;
    }
    if (kbd->fd >= 0)
    {
        close(kbd->fd);
    }
    delete kbd->path;
    xkb_state_unref(kbd->state);
    delete kbd;
}

void XkbWrapper::freeKeyboards(struct keyboard *kbds) const
{
    struct keyboard *next;

    while (kbds)
    {
        next = kbds->next;
        freeKeyboard(kbds);
        kbds = next;
    }
}
