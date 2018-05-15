#pragma once

#include "display/Display.hpp"

namespace display
{
  template<class SurfaceProvider>
  inline Display<SurfaceProvider>::Display()
    : instance{SurfaceProvider::getRequiredExtensiosn()}
    , surfaceProvider{instance}
    , device{}
    , swapchain{}
  {
    auto [physicalDevice, score] = instance.selectDevice([this](vk::PhysicalDevice physicalDevice)
							 {
							   struct Score
							   {
							     bool isSuitable;
							     unsigned int bestQueue;
							     vk::PhysicalDeviceType deviceType;

							     unsigned int deviceTypeScore() const
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

							   std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList(physicalDevice.getQueueFamilyProperties());
							   unsigned int bestQueueIndex = 0;
							   for (; bestQueueIndex < queueFamilyPropertiesList.size(); ++bestQueueIndex)
							     {
							       vk::QueueFamilyProperties const &queueFamilyProperties(queueFamilyPropertiesList[bestQueueIndex]);

							       if ((queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics) &&
								   surfaceProvider.getSurface().isQueueFamilySuitable(physicalDevice, bestQueueIndex))
								 {
								   break;
								 }
							     }
							   vk::PhysicalDeviceProperties properties(physicalDevice.getProperties());
							   return Score{bestQueueIndex != queueFamilyPropertiesList.size(), bestQueueIndex, properties.deviceType};
							 });
    if (!score.isSuitable)
      throw std::runtime_error("No suitable GPU found.");
    float priority[]{ 1.0 };
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, score.bestQueue, 1, priority};
    device = magma::Device<>(physicalDevice,
			     std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
			     std::vector<char const *>({VK_KHR_SWAPCHAIN_EXTENSION_NAME}));
    swapchain = magma::Swapchain<>(surfaceProvider.getSurface(), device, physicalDevice, swapchain);
  }
}
