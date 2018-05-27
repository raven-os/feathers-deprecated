#pragma once

#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <magma/VulkanHandler.hpp>
#include <magma/Surface.hpp>
#include <wayland-client.h>
#include <vector>

namespace  display
{
  class WaylandSurface
  {
    struct wl_display *wlDisplay;
    struct wl_compositor *wlCompositor;
    struct wl_surface *wlSurface;
    magma::Surface<> surface;
  public:
    static std::vector<char const *> getRequiredExtensiosn()
    {
      return {"VK_KHR_surface", "VK_KHR_wayland_surface"};
    }

    magma::Surface<claws::no_delete> getSurface()
    {
      return surface;
    }

    WaylandSurface(WaylandSurface const &) = delete;

    WaylandSurface(WaylandSurface &&other)
      : wlDisplay(other.wlDisplay)
      , wlCompositor(other.wlCompositor)
      , wlSurface(other.wlSurface)
      , surface(std::move(other.surface))
    {
      other.wlDisplay = nullptr;
      other.wlCompositor = nullptr;
      other.wlSurface = nullptr;
    }

    WaylandSurface(magma::Instance const &instance)
      : wlDisplay([](){
	  auto *display(wl_display_connect(NULL));

	  if (!display)
	    throw std::runtime_error("Could not connect to display");
	  return display;
	}())
      , wlCompositor([&](){
	  auto wlRegistry(wl_display_get_registry(wlDisplay));
	  wl_compositor *result(nullptr);
	  const struct wl_registry_listener registry_listener{[]
	      (void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version){
	      if (std::string_view("wl_compositor") == std::string_view(interface))
		{
		  *reinterpret_cast<wl_compositor **>(data) = reinterpret_cast<wl_compositor *>(wl_registry_bind(registry,
														 id,
														 &wl_compositor_interface,
														 1));
		}
	    }, [](void *data, struct wl_registry *registry, uint32_t id)
		 {
		   std::cout << "TODO\n";
		 }
	  };
	  wl_registry_add_listener(wlRegistry, &registry_listener, reinterpret_cast<void *>(&result));
	  wl_display_dispatch(wlDisplay);
	  wl_display_roundtrip(wlDisplay);
	  if (!result) {
	    throw std::runtime_error("Could not find compositor\n");
	  }
	  return result;
	}())
      , wlSurface([&](){
	  return wl_compositor_create_surface(wlCompositor);
	}())
      , surface(makeSurface(instance, [&](){
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
	  }()))
    {
    }

    ~WaylandSurface()
    {
      if (wlDisplay)
	{
	  wl_display_disconnect(wlDisplay);
	}
    }
  };
}
