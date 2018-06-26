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

namespace display
{
  class WindowTree;

  struct SwapchainUserData;
  struct FrameData;

  struct Renderer
  {
    magma::CommandPool<> commandPool;
    magma::DescriptorSetLayout<> descriptorSetLayout;
    magma::PipelineLayout<> pipelineLayout;
    magma::ShaderModule<> vert;
    magma::ShaderModule<> frag;
    vk::PhysicalDevice physicalDevice;
    magma::Semaphore<> renderDone;
    vk::Queue queue;
    magma::DescriptorPool<> descriptorPool;
    magma::DescriptorSets<> descriptorSets;
    magma::Image<> backgroundImage;
    magma::DeviceMemory<> backgroundImageMemory;
    magma::ImageView<> backgroundImageView;
    magma::DynamicBuffer stagingBuffer;
    magma::DynamicBuffer vertexBuffer;
    magma::Sampler<> sampler;

    Renderer(magma::Device<claws::no_delete> device, vk::PhysicalDevice physicalDevice, uint32_t selectedQueueFamily);
  
    ~Renderer() = default;

    magma::Semaphore<claws::no_delete> render(magma::Device<claws::no_delete> device, WindowTree const &windowTree, unsigned int index, SwapchainUserData &swapchainUserData, FrameData &frame, magma::Semaphore<claws::no_delete> imageAvailable);
    uint32_t prepareGpuData(FrameData &frame, WindowTree const &windowTree);


    vk::Extent2D getExtent() const noexcept
    {
      return vk::Extent2D{600, 400};
    }
  };

  struct SwapchainUserData
  {
    magma::CommandBufferGroup<magma::PrimaryCommandBuffer> commandBuffers;
    magma::RenderPass<> renderPass;
    magma::Pipeline<> pipeline;

    // This function handles pipeline creation
    magma::Pipeline<> createPipeline(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, Renderer const &renderer);

    SwapchainUserData() = default;
    SwapchainUserData(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, Renderer const &renderer, uint32_t imageCount)
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
		  vk::ImageLayout::ePresentSrcKHR});

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
      , pipeline(createPipeline(device, swapchain, renderer))
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

    FrameData(magma::Device<claws::no_delete> device,
	      magma::Swapchain<claws::no_delete> swapchain,
	      Renderer const &renderer,
	      SwapchainUserData &swapchainUserData,
	      magma::ImageView<claws::no_delete> swapchainImageView)
      : fence(device.createFence(vk::FenceCreateFlagBits::eSignaled))
      , depthImageMemory{}
      , depthImage([&]()
		   {
		     auto image(device.createImage2D({}, vk::Format::eD32Sfloat, {swapchain.getExtent().width, swapchain.getExtent().height}, vk::SampleCountFlagBits::e1,
						     vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageLayout::eUndefined));
		     auto memRequirements(device.getImageMemoryRequirements(depthImage));
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
    {
    }
  };
}
