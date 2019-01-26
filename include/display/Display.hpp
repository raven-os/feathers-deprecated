#pragma once

#include <unistd.h>

#include <magma/DisplaySystem.hpp>
#include <magma/VulkanHandler.hpp>
#include <magma/Device.hpp>

#include "display/Renderer.hpp"

namespace display
{
  class Subcompositor
  {
    struct Score
    {
      bool isSuitable;
      unsigned int bestQueue;
      vk::PhysicalDeviceType deviceType;

      unsigned int deviceTypeScore() const noexcept
      {
	switch(deviceType)
	  {
	  case vk::PhysicalDeviceType::eIntegratedGpu:
	    return 4;
	  case vk::PhysicalDeviceType::eDiscreteGpu:
	    return 3;
	  case vk::PhysicalDeviceType::eVirtualGpu:
	    return 2;
	  case vk::PhysicalDeviceType::eOther:
	    return 1;
	  case vk::PhysicalDeviceType::eCpu:
	  default:
	    return 0;
	  }
      }

      bool operator<(Score const &other) const noexcept
      {
	return (!isSuitable || (other.isSuitable && (deviceTypeScore() < other.deviceTypeScore())));
      }
    };
   
    magma::Device<> device;
    magma::Semaphore<> imageAvailable;
  public:
    magma::DisplaySystem<Renderer, SwapchainUserData, FrameData> displaySystem;

  private:

    Subcompositor(std::pair<vk::PhysicalDevice, Score> const &selectedResult, magma::Surface<claws::no_delete> surface)
      : device([this, &selectedResult, surface](){
	  float priority{1.0f};
	  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, selectedResult.second.bestQueue, 1, &priority};

	  return magma::Device<>(selectedResult.first,
				 std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
				 std::vector<char const *>({VK_KHR_SWAPCHAIN_EXTENSION_NAME}));
	}())
      , imageAvailable(device.createSemaphore())
      , displaySystem(selectedResult.first, surface, device, device.getQueue(selectedResult.second.bestQueue, 0u), selectedResult.second.bestQueue)
    {
    }

  public:
    Subcompositor(magma::Instance const &instance, magma::Surface<claws::no_delete> surface)
      : Subcompositor([&instance, surface](){
	  std::pair<vk::PhysicalDevice, Score>
	    result(instance.selectDevice([&instance, surface]
					 (vk::PhysicalDevice physicalDevice)
					 {
					   std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList(physicalDevice.getQueueFamilyProperties());
					   unsigned int bestQueueIndex = 0;
					   for (; bestQueueIndex < queueFamilyPropertiesList.size(); ++bestQueueIndex)
					     {
					       vk::QueueFamilyProperties const &queueFamilyProperties(queueFamilyPropertiesList[bestQueueIndex]);

					       if ((queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics) &&
						   surface.isQueueFamilySuitable(physicalDevice, bestQueueIndex))
						 {
						   break;
						 }
					     }
					   vk::PhysicalDeviceProperties properties(physicalDevice.getProperties());
					   return Score{bestQueueIndex != queueFamilyPropertiesList.size(), bestQueueIndex, properties.deviceType};
					 }));
	  if (!result.second.isSuitable)
	    {
	      throw std::runtime_error("No suitable GPU found.");
	    }
	  return result;
	}(), surface)
    {
    }

    void render(wm::WindowTree const &windowTree);
  };

  class Display
  {
    magma::Instance instance;
    magma::Surface<> surface;
    Subcompositor renderer;

  public:
    template<class SurfaceProvider>
    Display(SurfaceProvider &surfaceProvider)
      : instance{[](){
	auto out(SurfaceProvider::getRequiredExtensions());
     
	std::cout << "required:" << std::endl;
	for (auto const &name : out) {
	  std::cout << name << std::endl;
	}
	auto available(vk::enumerateInstanceExtensionProperties(nullptr));

	std::cout << "available:" << std::endl;
	for (auto const &ext : available) {
	  std::cout << ext.extensionName << std::endl;
	}
	return out;
      }()}
      , surface(surfaceProvider.createSurface(instance))
      , renderer(instance, surface)
    {
    }

    Display(Display const &) = delete;
    Display(Display &&) = delete;
    Display operator=(Display const &) = delete;
    Display operator=(Display &&) = delete;

    Renderer &getRenderer() noexcept
    {
      return renderer.displaySystem.userData;
    }

    Renderer const &getRenderer() const noexcept
    {
      return renderer.displaySystem.userData;
    }

    void render(wm::WindowTree const &windowTree)
    {
      renderer.render(windowTree);
    }
  };
}
