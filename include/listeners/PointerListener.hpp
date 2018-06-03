#pragma once

#include <wayland-client.hpp>
#include <iostream>

struct PointerListener
{
    PointerListener() = default;
    ~PointerListener() = default;

    void pointer_enter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY);
    void pointer_leave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
    void pointer_motion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    void pointer_button(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    void pointer_axis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
};
