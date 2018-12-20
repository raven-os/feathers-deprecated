#pragma once

#include <vector>

#include <magma/DisplaySystem.hpp>
#include <magma/VulkanHandler.hpp>
#include <magma/Device.hpp>

#include "display/Renderer.hpp"
#include "opengl/QuadFullscreen.hpp"
#include "modeset/ModeSetter.hpp"

namespace display
{
  struct Score
  {
    unsigned int bestQueueIndex;
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
  };

  inline bool operator<(std::optional<Score> const &lh, std::optional<Score> const &rh) noexcept
  {
    return (!lh || (rh && (lh->deviceTypeScore() < rh->deviceTypeScore())));
  }

  class Compositor
  {
    struct Swapchain
    {
      modeset::ModeSetter const *modeset;
     
      constexpr vk::Format getFormat() const noexcept
      {
	return vk::Format::eR8G8B8A8Srgb;
      }

      vk::Extent2D getExtent() const
      {
	return {uint32_t(modeset->getScreenWidth()), uint32_t(modeset->getScreenHeight())};
      }
    };

    modeset::ModeSetter modeSetter;
    QuadFullscreen quadFullscreen;
    magma::Device<> device;
    magma::Semaphore<> imageAvailable;
    magma::Fence<> fence;
    Renderer renderer;
    SwapchainUserData swapchainUserData;
    static constexpr uint32_t imageCount{2u};
    unsigned int frameIndex{0u};
    magma::CommandBufferGroup<magma::PrimaryCommandBuffer> commandBuffer;

    struct Frame
    {
      magma::DeviceMemory<> imageMemory; // keep image memory seperate to be able to map it in concurently.
      magma::Image<> image;
      magma::ImageView<> imageView;
      FrameData data;

      Frame(magma::Device<claws::no_delete> device, Swapchain const &swapchain, Renderer &userData, SwapchainUserData &swapchainUserData,  vk::PhysicalDevice physicalDevice)
	: imageMemory{}
	, image([&](){
	    auto image(device.createImage2D({}, swapchain.getFormat(), {swapchain.getExtent().width, swapchain.getExtent().height}, vk::SampleCountFlagBits::e1,
					    vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageLayout::eUndefined));
	    auto memRequirements(device.getImageMemoryRequirements(image));
	    imageMemory = device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits);

	    device.bindImageMemory(image, imageMemory, 0);
	    return image;
	  }())
	, imageView(device.createImageView({},
					   image,
					   vk::ImageViewType::e2D,
					   swapchain.getFormat(),
					   {vk::ComponentSwizzle::eIdentity,
					       vk::ComponentSwizzle::eIdentity,
					       vk::ComponentSwizzle::eIdentity,
					       vk::ComponentSwizzle::eIdentity},
					   {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}))

	, data{device, swapchain, userData, swapchainUserData, imageView}
      {
      }
    };
    std::array<Frame, imageCount> frames;

    Compositor(std::pair<vk::PhysicalDevice, Score> const &selectedResult)
      : device([this, &selectedResult](){
	  float priority{1.0f};
	  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, selectedResult.second.bestQueueIndex, 1, &priority};

	  return magma::Device<>(selectedResult.first,
				 std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
				 std::vector<char const *>({"VK_KHR_external_memory", "VK_KHR_external_memory_fd"}));
	}())
      , imageAvailable(device.createSemaphore())
      , fence(device.createFence({}))
      , renderer(device, selectedResult.first, selectedResult.second.bestQueueIndex)
      , swapchainUserData(device, Swapchain{&modeSetter}, renderer, imageCount, vk::ImageLayout::eGeneral)
      , commandBuffer(renderer.commandPool.allocatePrimaryCommandBuffers(1))
      , frames(claws::init_array<imageCount>([&]()
					     {
					       return Frame{device, Swapchain{&modeSetter}, renderer, swapchainUserData, selectedResult.first};
					     }))
    {
    }

  public:
    Compositor(magma::Instance const &instance)
      : Compositor([&instance](){
	  std::pair<vk::PhysicalDevice, std::optional<Score>>
	    result(instance.selectDevice([&instance]
					 (vk::PhysicalDevice physicalDevice)
					 {
					   std::vector<vk::ExtensionProperties> availableExtensions(physicalDevice.enumerateDeviceExtensionProperties());
					   std::vector<char const *> requiredExtensions({"VK_KHR_external_memory", "VK_KHR_external_memory_fd"});

					   std::cout << "available device extensions:\n";
					   for (auto const &extension : availableExtensions)
					     {
					       std::cout << "\"" << extension.extensionName << "\", version " << extension.specVersion << "\n";

					       auto it(std::find_if(requiredExtensions.begin(), requiredExtensions.end(),
								    [&extension](char const *str) noexcept
								    {
								      return !std::strcmp(extension.extensionName, str);
								    }));

					       if (it != requiredExtensions.end())
						 requiredExtensions.erase(it);
					     }
					   if (!requiredExtensions.empty())
					     return std::optional<Score>{};

					   std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList(physicalDevice.getQueueFamilyProperties());
					   unsigned int bestQueueIndex = 0;
					   for (; bestQueueIndex < queueFamilyPropertiesList.size(); ++bestQueueIndex)
					     {
					       vk::QueueFamilyProperties const &queueFamilyProperties(queueFamilyPropertiesList[bestQueueIndex]);

					       if (queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
						 {
						   break;
						 }
					     }
					   if (bestQueueIndex == queueFamilyPropertiesList.size())
					     return std::optional<Score>{};
					   vk::PhysicalDeviceProperties properties(physicalDevice.getProperties());
					   return std::optional<Score>{Score{bestQueueIndex, properties.deviceType}};
					 }));
	  if (!result.second)
	    {
	      throw std::runtime_error("No suitable GPU found.");
	    }
	  return std::pair<vk::PhysicalDevice, Score>{result.first, *result.second};
	}())
    {
    }

    void render(wm::WindowTree const &windowTree);
    modeset::ModeSetter const& getModeSetter() const noexcept;
  };

  class KernelDisplay
  {
    magma::Instance instance;
    Compositor renderer;

  public:
    KernelDisplay()
      : instance{[](){
	std::vector<char const *> out{"VK_KHR_get_physical_device_properties2", "VK_KHR_external_memory_capabilities"};
     
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
      , renderer(instance)
    {
    }

    KernelDisplay(KernelDisplay const &) = delete;
    KernelDisplay(KernelDisplay &&) = delete;
    KernelDisplay operator=(KernelDisplay const &) = delete;
    KernelDisplay operator=(KernelDisplay &&) = delete;

    modeset::ModeSetter const& getModeSetter() const noexcept;

    void render(wm::WindowTree const &windowTree)
    {
      renderer.render(windowTree);
    }
  };
}
