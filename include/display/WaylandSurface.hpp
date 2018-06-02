#pragma once

#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <magma/VulkanHandler.hpp>
#include <magma/Surface.hpp>
#include <wayland-client.h>
#include <vector>
#include <xkbcommon/xkbcommon.h>
#include <sys/mman.h>
#include <unistd.h>

#include "WaylandAdapters.hpp"

namespace  display
{
  class WaylandSurface
  {
    struct wl_display *wlDisplay{nullptr};
    struct wl_registry *wlRegistry{nullptr};
    struct wl_compositor *wlCompositor{nullptr};
    struct wl_surface *wlSurface{nullptr};
    struct wl_shell *wlShell{nullptr};
    struct wl_shell_surface *wlShellSurface{nullptr};
    struct wl_seat *wlSeat{nullptr};

    struct xkb_context *xkb_context;
    struct xkb_keymap *keymap{nullptr};
    struct xkb_state *xkb_state{nullptr};

    bool running = true;
  public:
    WaylandSurface(WaylandSurface const &) = delete;

    WaylandSurface(WaylandSurface &&other) = delete;

    WaylandSurface();

    static std::vector<char const *> getRequiredExtensiosn()
    {
      return {"VK_KHR_surface", "VK_KHR_wayland_surface"};
    }

    magma::Surface<> createSurface(magma::Instance const &instance)
    {
      return makeSurface(instance, [&](){
	  VkWaylandSurfaceCreateInfoKHR waylnadSurfaceCreateInfo
	    {
	      VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		wlDisplay,
		wlSurface
		};
	    VkSurfaceKHR surface;
	    vkCreateWaylandSurfaceKHR(instance.vkInstance, &waylnadSurfaceCreateInfo, nullptr, &surface);
	    return surface;
	}());
    }

    ~WaylandSurface() = default;

    void pointer_enter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surfX, wl_fixed_t surfY);
    void pointer_leave(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
    void pointer_motion(struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    void pointer_button(struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    void pointer_axis(struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

    void keyboard_keymap(struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
    void keyboard_enter(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
    void keyboard_leave(struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
    void keyboard_key(struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    void keyboard_modifiers(struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);

    void seat_capabilities(struct wl_seat *seat, uint32_t capabilities);
    void seat_name(struct wl_seat *seat, const char *name);

    void shell_surface_ping(struct wl_shell_surface *shell_surface, uint32_t serial);
    void shell_surface_configure(struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    void shell_surface_popup_done (struct wl_shell_surface *shell_surface);

    void registry_add_object(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    void registry_remove_object (struct wl_registry *registry, uint32_t name);

    void dispatch();
    bool isRunning() const;
  };
}
