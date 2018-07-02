#include "EvdevClient.hpp"

EvdevClient::EvdevClient()
:
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
    epfd = epoll_create1(0);
    if (epfd < 0)
    {
        fprintf(stderr, "Couldn't create epoll instance: %s\n", strerror(errno));
        return (valid);
    }
    destructionFlag = toDestroy::FROM_EPFD;
    for (struct keyboard *kbd = kbds; kbd; kbd = kbd->next)
    {
        struct epoll_event ev;

        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;
        ev.data.ptr = kbd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, kbd->fd, &ev))
        {
            fprintf(stderr, "Couldn't add %s to epoll: %s\n", kbd->path, strerror(errno));
            return (valid);
        }
    }
    valid = true;
    return (valid);
}

void EvdevClient::destroyClient()
{
    switch (destructionFlag)
    {
        case toDestroy::ALL:
        case toDestroy::FROM_EPFD:
        close(epfd);
        [[fallthrough]];
        case toDestroy::FROM_KEYBOARDS:
        xkbWrapper.freeKeyboards(kbds);
        [[fallthrough]];
        case toDestroy::FROM_KEYMAP:
        xkb_keymap_unref(keymap);
        [[fallthrough]];
        case toDestroy::ONLY_CONTEXT:
        xkb_context_unref(ctx);
        default:
        break;
    }
}

bool EvdevClient::isValid() const
{
    return (valid);
}

void test_print_keycode_state(struct xkb_state *state,
    xkb_keycode_t keycode,
    enum xkb_consumed_mode consumed_mode
)
{
    struct xkb_keymap *keymap;
    const xkb_keysym_t *syms;
    int nsyms;
    char s[16];
    xkb_layout_index_t layout;

    keymap = xkb_state_get_keymap(state);
    nsyms = xkb_state_key_get_syms(state, keycode, &syms);
    if (nsyms <= 0)
    {
        return;
    }
    printf("\nkeysyms [ ");
    for (int i = 0; i < nsyms; i++)
    {
        xkb_keysym_get_name(syms[i], s, sizeof(s));
        printf("%-*s ", (int) sizeof(s), s);
    }
    printf("] ");
    xkb_state_key_get_utf8(state, keycode, s, sizeof(s));
    printf("unicode [ %s ] ", s);
    layout = xkb_state_key_get_layout(state, keycode);
    printf("layout [ %s (%d) ] ",
    xkb_keymap_layout_get_name(keymap, layout), layout);
    printf("level [ %d ] ",
    xkb_state_key_get_level(state, keycode, layout));
    printf("mods [ ");
    for (xkb_mod_index_t mod = 0; mod < xkb_keymap_num_mods(keymap); mod++)
    {
        if (xkb_state_mod_index_is_active(state, mod, XKB_STATE_MODS_EFFECTIVE) <= 0)
        {
            continue;
        }
        if (xkb_state_mod_index_is_consumed2(state, keycode, mod, consumed_mode))
        {
            printf("-%s ", xkb_keymap_mod_get_name(keymap, mod));
        }
        else
        {
            printf("%s ", xkb_keymap_mod_get_name(keymap, mod));
        }
    }
    printf("] ");
    printf("leds [ ");
    for (xkb_led_index_t led = 0; led < xkb_keymap_num_leds(keymap); led++)
    {
        if (xkb_state_led_index_is_active(state, led) <= 0)
        {
            continue;
        }
        printf("%s ", xkb_keymap_led_get_name(keymap, led));
    }
    printf("] ");
    printf("\n");
}

void EvdevClient::processEvent(struct keyboard *kbd,
    uint16_t type,
    uint16_t code,
    int32_t value
)
{
    xkb_keycode_t keycode;
    struct xkb_keymap *keymap;

    if (type != EV_KEY)
    {
        return;
    }
    keycode = EVDEV_OFFSET + code;
    keymap = xkb_state_get_keymap(kbd->state);
    if (value == keyState::KEY_STATE_REPEAT && !xkb_keymap_key_repeats(keymap, keycode))
    {
        return;
    }
    if (value != keyState::KEY_STATE_RELEASE)
    {
        test_print_keycode_state(kbd->state,
            keycode,
            XKB_CONSUMED_MODE_XKB
        );
    }
    if (value == keyState::KEY_STATE_RELEASE)
    {
        xkb_state_update_key(kbd->state, keycode, XKB_KEY_UP);
    }
    else
    {
        xkb_state_update_key(kbd->state, keycode, XKB_KEY_DOWN);
    }
}

int EvdevClient::readKeyboard(struct keyboard *kbd)
{
    ssize_t len;
    struct input_event evs[16];

    while ((len = read(kbd->fd, &evs, sizeof(evs))) > 0)
    {
        const size_t nevs = len / sizeof(struct input_event);

        for (size_t i = 0; i < nevs; i++)
        {
            processEvent(kbd, evs[i].type, evs[i].code, evs[i].value);
        }
    }
    if (len < 0 && errno != EWOULDBLOCK)
    {
        fprintf(stderr, "Couldn't read %s: %s\n", kbd->path, strerror(errno));
        return (1);
    }
    return (0);
}

void EvdevClient::tick()
{
    int ret;
    struct keyboard *kbd;
    struct epoll_event evs[16];

    ret = epoll_wait(epfd, evs, 16, 10);
    if (ret < 0)
    {
        if (errno == EINTR)
        {
            return;
        }
        fprintf(stderr, "Couldn't poll for events: %s\n", strerror(errno));
    }
    for (int i = 0; i < ret; i++)
    {
        kbd = (struct keyboard *)evs[i].data.ptr;
        if (readKeyboard(kbd))
        {
            return;
        }
    }
    return;
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
        case toDestroy::FROM_EPFD:
        close(epfd);
        [[fallthrough]];
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

/* The meaning of the input_event 'value' field. */
enum {
    KEY_STATE_RELEASE = 0,
    KEY_STATE_PRESS = 1,
    KEY_STATE_REPEAT = 2,
};

void test_print_keycode_state(struct xkb_state *state,
    xkb_keycode_t keycode,
    enum xkb_consumed_mode consumed_mode
)
{
    struct xkb_keymap *keymap;
    const xkb_keysym_t *syms;
    int nsyms;
    char s[16];
    xkb_layout_index_t layout;

    keymap = xkb_state_get_keymap(state);
    nsyms = xkb_state_key_get_syms(state, keycode, &syms);
    if (nsyms <= 0)
    {
        return;
    }
    printf("\nkeysyms [ ");
    for (int i = 0; i < nsyms; i++)
    {
        xkb_keysym_get_name(syms[i], s, sizeof(s));
        printf("%-*s ", (int) sizeof(s), s);
    }
    printf("] ");
    xkb_state_key_get_utf8(state, keycode, s, sizeof(s));
    printf("unicode [ %s ] ", s);
    layout = xkb_state_key_get_layout(state, keycode);
    printf("layout [ %s (%d) ] ",
    xkb_keymap_layout_get_name(keymap, layout), layout);
    printf("level [ %d ] ",
    xkb_state_key_get_level(state, keycode, layout));
    printf("mods [ ");
    for (xkb_mod_index_t mod = 0; mod < xkb_keymap_num_mods(keymap); mod++)
    {
        if (xkb_state_mod_index_is_active(state, mod, XKB_STATE_MODS_EFFECTIVE) <= 0)
        {
            continue;
        }
        if (xkb_state_mod_index_is_consumed2(state, keycode, mod, consumed_mode))
        {
            printf("-%s ", xkb_keymap_mod_get_name(keymap, mod));
        }
        else
        {
            printf("%s ", xkb_keymap_mod_get_name(keymap, mod));
        }
    }
    printf("] ");
    printf("leds [ ");
    for (xkb_led_index_t led = 0; led < xkb_keymap_num_leds(keymap); led++)
    {
        if (xkb_state_led_index_is_active(state, led) <= 0)
        {
            continue;
        }
        printf("%s ", xkb_keymap_led_get_name(keymap, led));
    }
    printf("] ");
    printf("\n");
}

void EvdevClient::processEvent(struct keyboard *kbd,
    uint16_t type,
    uint16_t code,
    int32_t value
)
{
    xkb_keycode_t keycode;
    struct xkb_keymap *keymap;

    if (type != EV_KEY)
    {
        return;
    }
    keycode = EVDEV_OFFSET + code;
    keymap = xkb_state_get_keymap(kbd->state);
    if (value == keyState::KEY_STATE_REPEAT && !xkb_keymap_key_repeats(keymap, keycode))
    {
        return;
    }
    if (value != keyState::KEY_STATE_RELEASE)
    {
        test_print_keycode_state(kbd->state,
            keycode,
            XKB_CONSUMED_MODE_XKB
        );
    }
    if (value == keyState::KEY_STATE_RELEASE)
    {
        xkb_state_update_key(kbd->state, keycode, XKB_KEY_UP);
    }
    else
    {
        xkb_state_update_key(kbd->state, keycode, XKB_KEY_DOWN);
    }
}

int EvdevClient::readKeyboard(struct keyboard *kbd)
{
    ssize_t len;
    struct input_event evs[16];

    while ((len = read(kbd->fd, &evs, sizeof(evs))) > 0)
    {
        const size_t nevs = len / sizeof(struct input_event);

        for (size_t i = 0; i < nevs; i++)
        {
            processEvent(kbd, evs[i].type, evs[i].code, evs[i].value);
        }
    }
    if (len < 0 && errno != EWOULDBLOCK)
    {
        fprintf(stderr, "Couldn't read %s: %s\n", kbd->path, strerror(errno));
        return (1);
    }
    return (0);
}

void EvdevClient::tick()
{
    int ret;
    struct keyboard *kbd;
    struct epoll_event evs[16];

    ret = epoll_wait(epfd, evs, 16, 10);
    if (ret < 0)
    {
        if (errno == EINTR)
        {
            return;
        }
        fprintf(stderr, "Couldn't poll for events: %s\n", strerror(errno));
    }
    for (int i = 0; i < ret; i++)
    {
        kbd = (struct keyboard *)evs[i].data.ptr;
        if (readKeyboard(kbd))
        {
            return;
        }
    }
    return;
}
