#pragma once

#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <magma/VulkanHandler.hpp>
#include <magma/Surface.hpp>
#include <wayland-client.h>
#include <vector>

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

    void shell_surface_ping(struct wl_shell_surface *shell_surface, uint32_t serial);
    void shell_surface_configure(struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    void shell_surface_popup_done (struct wl_shell_surface *shell_surface);

    void registry_add_object(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    void registry_remove_object (struct wl_registry *registry, uint32_t name);

    void dispatch();
  };
}
