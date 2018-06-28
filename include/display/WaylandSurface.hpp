#pragma once

#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <magma/VulkanHandler.hpp>
#include <magma/Surface.hpp>
#include <vector>

#include "UserInput.hpp"
#include "listeners/SeatListener.hpp"

namespace display
{
  class WaylandSurface : public UserInput
  {
    struct wl_registry *wlRegistry{nullptr};
    struct wl_compositor *wlCompositor{nullptr};
    struct wl_surface *wlSurface{nullptr};
    struct wl_shell *wlShell{nullptr};
    struct wl_shell_surface *wlShellSurface{nullptr};
    struct wl_seat *wlSeat{nullptr};

    wayland_client::SeatListener *seatListener;

  public:
    WaylandSurface(WaylandSurface const &) = delete;

    WaylandSurface(WaylandSurface &&other) = delete;

    WaylandSurface();
    WaylandSurface(std::string const &displayname="");

    static std::vector<char const *> getRequiredExtensions()
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

    void shellSurfacePing(struct wl_shell_surface *shellSurface, uint32_t serial);
    void shellSurfaceConfigure(struct wl_shell_surface *shellSurface, uint32_t edges, int32_t width, int32_t height);
    void shellSurfacePopupDone (struct wl_shell_surface *shellSurface);

    void registryAddObject(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    void registryRemoveObject (struct wl_registry *registry, uint32_t name);

    bool isRunning() const;
  };
}
