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
    struct wl_display *wlDisplay;
    struct wl_registry *wlRegistry;
    struct wl_compositor *wlCompositor;
    struct wl_surface *wlSurface;
    struct wl_shell *wlShell;
    struct wl_seat *wlSeat;
  public:
    WaylandSurface(WaylandSurface const &) = delete;

    WaylandSurface(WaylandSurface &&other)
      : wlDisplay(other.wlDisplay)
      , wlCompositor(other.wlCompositor)
      , wlSurface(other.wlSurface)
    {
      other.wlDisplay = nullptr;
      other.wlCompositor = nullptr;
      other.wlSurface = nullptr;
    }

    WaylandSurface()
      : wlDisplay(wl_display_connect(NULL))
    {
      if (!wlDisplay)
	throw std::runtime_error("Could not connect to display");
      wlRegistry = wl_display_get_registry(wlDisplay);
      addListener(wlRegistry, *this);
      wl_display_dispatch(wlDisplay);
      wl_display_roundtrip(wlDisplay);
      if (!wlCompositor) {
	throw std::runtime_error("Could not find compositor\n");
      }

      wlSurface = wl_compositor_create_surface(wlCompositor); 
    }

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

    ~WaylandSurface()
    {
      if (wlDisplay)
	{
	  wl_display_disconnect(wlDisplay);
	}
    }

    void dispatch()
    {
      wl_display_dispatch_pending(wlDisplay);
    }

    void shell_surface_ping (struct wl_shell_surface *shell_surface, uint32_t serial)
    {
      wl_shell_surface_pong (shell_surface, serial);
    }

    void shell_surface_configure (struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
    {
    }

    void shell_surface_popup_done (struct wl_shell_surface *shell_surface)
    {
    }

    void registry_add_object (struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
    {
      if (!strcmp(interface,"wl_compositor"))
	{
	  wlCompositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
	}
      else if (!strcmp(interface,"wl_shell"))
	{
	  wlShell = static_cast<wl_shell *>(wl_registry_bind (registry, name, &wl_shell_interface, 1));
	}
      else if (!strcmp(interface,"wl_seat"))
	{
	  wlSeat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
	  // TODO: convert to new style
	  // wl_seat_add_listener(UserInput::get().seat,
	  // 		     &std::get<struct wl_seat_listener>(
	  // 							UserInput::get().listeners->getListener("seat")),
	  // 		     NULL);
	}
    }

    void registry_remove_object (struct wl_registry *registry, uint32_t name) {

    }


  };
}
