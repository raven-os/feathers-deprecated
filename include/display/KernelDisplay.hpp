#pragma once

#include <magma/DisplaySystem.hpp>
#include <magma/VulkanHandler.hpp>
#include <magma/Device.hpp>

#include <claws/array_ops.hpp>

#include "display/Renderer.hpp"
#include "opengl/QuadFullscreen.hpp"
#include "modeset/ModeSetter.hpp"

namespace display
{
  class WindowTree;

  class Compositor
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

    struct Swapchain
    {
      ModeSetter const *modeset;
      
      constexpr vk::Format getFormat() const noexcept
      {
	return vk::Format::eR8G8B8A8Srgb;
      }

      vk::Extent2D getExtent() const
      {
	return {modeset->getScreenWidth(), modeset->getScreenHeight()};
      }
    };

    ModeSetter modeSetter;
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
	    std::cout << "image data size: " << memRequirements.size << std::endl;

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
    bool frame;


    Compositor(std::pair<vk::PhysicalDevice, Score> const &selectedResult)
      : device([this, &selectedResult](){
	  float priority{1.0f};
	  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, selectedResult.second.bestQueue, 1, &priority};

	  return magma::Device<>(selectedResult.first,
				 std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
				 std::vector<char const *>({VK_KHR_SWAPCHAIN_EXTENSION_NAME}));
	}())
      , imageAvailable(device.createSemaphore())
      , fence(device.createFence({}))
      , renderer(device, selectedResult.first, selectedResult.second.bestQueue)
      , swapchainUserData(device, Swapchain{&modeSetter}, renderer, imageCount, vk::ImageLayout::eGeneral)
      , frames(claws::init_array<imageCount>([&]()
					     {
					       return Frame{device, Swapchain{&modeSetter}, renderer, swapchainUserData, selectedResult.first};
					     }))
      , commandBuffer(renderer.commandPool.allocatePrimaryCommandBuffers(1))
    {
    }

  public:
    Compositor(magma::Instance const &instance)
      : Compositor([&instance](){
	  std::pair<vk::PhysicalDevice, Score>
	    result(instance.selectDevice([&instance]
					 (vk::PhysicalDevice physicalDevice)
					 {
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
					   vk::PhysicalDeviceProperties properties(physicalDevice.getProperties());
					   return Score{bestQueueIndex != queueFamilyPropertiesList.size(), bestQueueIndex, properties.deviceType};
					 }));
	  if (!result.second.isSuitable)
	    {
	      throw std::runtime_error("No suitable GPU found.");
	    }
	  return result;
	}())
    {
    }

    void render(WindowTree const &windowTree);
  };

  class KernelDisplay
  {
    magma::Instance instance;
    Compositor renderer;

  public:
    KernelDisplay()
      : instance{}
      , renderer(instance)
    {
    }

    KernelDisplay(KernelDisplay const &) = delete;
    KernelDisplay(KernelDisplay &&) = delete;
    KernelDisplay operator=(KernelDisplay const &) = delete;
    KernelDisplay operator=(KernelDisplay &&) = delete;

    void render(WindowTree const &windowTree)
    {
      renderer.render(windowTree);
    }
  };
}
