#pragma once

#include <fstream>

#include <magma/Device.hpp>
#include <magma/Swapchain.hpp>
#include <magma/Fence.hpp>
#include <magma/Semaphore.hpp>
#include <magma/CreateInfo.hpp>
#include <magma/ShaderModule.hpp>
#include <magma/DescriptorSetLayout.hpp>
#include <magma/Buffer.hpp>
#include <magma/DeviceMemory.hpp>
#include <magma/Framebuffer.hpp>
#include <magma/DescriptorSets.hpp>
#include <magma/DescriptorSetLayout.hpp>
#include <magma/Image.hpp>
#include <magma/Sampler.hpp>
#include <magma/DynamicBuffer.hpp>
#include <magma/CommandBuffer.hpp>
#include <magma/ImageView.hpp>

#include "display/SuperCorbeau.hpp"

namespace wm
{
  class WindowTree;
}

namespace protocol
{
  class Buffer;
}

struct wl_resource;

namespace display
{

  struct SwapchainUserData;
  struct FrameData;

  struct Renderer
  {
    magma::CommandPool<> commandPool;
    magma::DescriptorSetLayout<> descriptorSetLayout;
    magma::DescriptorSetLayout<> samplerDescriptorSetLayout;
    magma::PipelineLayout<> pipelineLayout;
    magma::ShaderModule<> vert;
    magma::ShaderModule<> frag;
    vk::PhysicalDevice physicalDevice;
    magma::Semaphore<> renderDone;
    vk::Queue queue;
    magma::DescriptorPool<> samplerDescriptorPool;
    magma::DescriptorSets<> samplerDescriptorSet;
    magma::DeviceMemory<> backgroundImageMemory;
    magma::Image<> backgroundImage;
    magma::ImageView<> backgroundImageView;
    magma::DynamicBuffer stagingBuffer;
    magma::DynamicBuffer vertexBuffer;
    magma::Sampler<> sampler;
    magma::Device<claws::no_delete> device;

    Renderer(magma::Device<claws::no_delete> device, vk::PhysicalDevice physicalDevice, uint32_t selectedQueueFamily);
 
    ~Renderer() = default;

    magma::Semaphore<claws::no_delete> render(magma::Device<claws::no_delete> device, wm::WindowTree const &windowTree, unsigned int index, SwapchainUserData &swapchainUserData, FrameData &frame, magma::Semaphore<claws::no_delete> imageAvailable);
    void uploadBuffer(magma::DynamicBuffer::RangeId buffer, magma::Image<claws::no_delete> image, uint32_t width, uint32_t height);
    std::vector<magma::ImageView<claws::no_delete>> prepareGpuData(FrameData &frame, wm::WindowTree const &windowTree, std::vector<std::pair<struct wl_resource *, protocol::Buffer *>> &);


    vk::Extent2D getExtent() const noexcept
    {
      return vk::Extent2D{1920, 1080};
    }

    magma::DynamicBuffer &getStagingBuffer() noexcept
    {
      return stagingBuffer;
    }

    magma::Device<claws::no_delete> getDevice() const noexcept
    {
      return device;
    }

    vk::PhysicalDevice getPhysicalDevice() const noexcept
    {
      return physicalDevice;
    }
  };

  struct SwapchainUserData
  {
    magma::CommandBufferGroup<magma::PrimaryCommandBuffer> commandBuffers;
    magma::RenderPass<> renderPass;
    magma::Pipeline<> pipeline;

    // This function handles pipeline creation
    magma::Pipeline<> createPipeline(magma::Device<claws::no_delete> device, vk::Extent2D extent, Renderer const &renderer);

    SwapchainUserData() = default;

    template<class Swapchain>
    SwapchainUserData(magma::Device<claws::no_delete> device, Swapchain swapchain, Renderer const &renderer, uint32_t imageCount, vk::ImageLayout const presentLayout = vk::ImageLayout::ePresentSrcKHR)
      : commandBuffers(renderer.commandPool.allocatePrimaryCommandBuffers(imageCount))
      , renderPass([&](){
	  magma::RenderPassCreateInfo renderPassCreateInfo{{}};

	  // We have a simple renderpass writting to an image.
	  // The attached framebuffer will be cleared
	  renderPassCreateInfo.attachements.push_back({{},
		swapchain.getFormat(),
		  vk::SampleCountFlagBits::e1,
		  vk::AttachmentLoadOp::eClear,
		  vk::AttachmentStoreOp::eStore,
		  vk::AttachmentLoadOp::eDontCare,
		  vk::AttachmentStoreOp::eDontCare,
		  vk::ImageLayout::eUndefined,
		  presentLayout});

	  renderPassCreateInfo.attachements.push_back({{},
		vk::Format::eD32Sfloat,
		  vk::SampleCountFlagBits::e1,
		  vk::AttachmentLoadOp::eClear,
		  vk::AttachmentStoreOp::eStore,
		  vk::AttachmentLoadOp::eDontCare,
		  vk::AttachmentStoreOp::eDontCare,
		  vk::ImageLayout::eUndefined,
		  vk::ImageLayout::eDepthStencilAttachmentOptimal});

	  vk::AttachmentReference colorAttachmentReferences(0, vk::ImageLayout::eColorAttachmentOptimal);
	  vk::AttachmentReference depthAttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);


	  renderPassCreateInfo.subPasses.push_back(magma::StructBuilder<vk::SubpassDescription, true>
						   ::make(vk::PipelineBindPoint::eGraphics,
							  magma::EmptyList{},
							  magma::asListRef(colorAttachmentReferences),
							  nullptr,
							  &depthAttachmentReference,
							  magma::EmptyList{}));

	  return device.createRenderPass(renderPassCreateInfo);
	}())
      , pipeline(createPipeline(device, swapchain.getExtent(), renderer))
    {
    }

    SwapchainUserData(magma::Device<claws::no_delete> device, magma::Swapchain<> const &swapchain, Renderer const &renderer, uint32_t imageCount)
      : SwapchainUserData(device, magma::Swapchain<claws::no_delete>(swapchain), renderer, imageCount)
    {
    }
  }; 

  struct FrameData
  {
    magma::Fence<> fence;
    magma::DeviceMemory<> depthImageMemory;
    magma::Image<> depthImage;
    magma::ImageView<> depthImageView;
    magma::Framebuffer<> framebuffer;
    magma::DynamicBuffer::RangeId vertexBufferRangeId{magma::DynamicBuffer::nullId};
    magma::DynamicBuffer::RangeId indexBufferRangeId{magma::DynamicBuffer::nullId};
    uint32_t descriptorPoolSize;
    magma::DescriptorPool<> descriptorPool;
    std::vector<magma::DescriptorSets<>> descriptorSets;
    std::vector<std::pair<struct wl_resource *, protocol::Buffer *>> buffersInUse;

    template<class Swapchain>
    FrameData(magma::Device<claws::no_delete> device,
	      Swapchain swapchain,
	      Renderer const &renderer,
	      SwapchainUserData &swapchainUserData,
	      magma::ImageView<claws::no_delete> swapchainImageView)
      : fence(device.createFence(vk::FenceCreateFlagBits::eSignaled))
      , depthImageMemory{}
      , depthImage([&]()
		   {
		     auto image(device.createImage2D({}, vk::Format::eD32Sfloat, {swapchain.getExtent().width, swapchain.getExtent().height}, vk::SampleCountFlagBits::e1,
						     vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageLayout::eUndefined));
		     auto memRequirements(device.getImageMemoryRequirements(image));
		     depthImageMemory = device.selectAndCreateDeviceMemory(renderer.physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits);

		     device.bindImageMemory(image, depthImageMemory, 0);
		     return image;
		   }())
      , depthImageView(device.createImageView({},
					      depthImage,
					      vk::ImageViewType::e2D,
					      vk::Format::eD32Sfloat,
					      vk::ComponentMapping{},
					      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth,
									0,
									1,
									0,
									1)))
      , framebuffer(device.createFramebuffer(swapchainUserData.renderPass,
					     std::vector<vk::ImageView>{swapchainImageView, depthImageView},
					     swapchain.getExtent().width,
					     swapchain.getExtent().height,
					     1))
      , descriptorPoolSize(0u)
    {
    }

    FrameData(magma::Device<claws::no_delete> device,
	      magma::Swapchain<> const &swapchain,
	      Renderer const &renderer,
	      SwapchainUserData &swapchainUserData,
	      magma::ImageView<claws::no_delete> swapchainImageView)
      : FrameData(device, magma::Swapchain<claws::no_delete>(swapchain), renderer, swapchainUserData, swapchainImageView)
    {
    }
  };
}
