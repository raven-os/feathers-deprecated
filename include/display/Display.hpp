#pragma once

#include <unistd.h>

#include <magma/DisplaySystem.hpp>
#include <magma/VulkanHandler.hpp>
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

#include "display/SuperCorbeau.hpp"

namespace display
{
  class WindowTree;

  class Display
  {
    struct Renderer
    {
      struct UserData
      {
	magma::CommandPool<> commandPool;
	magma::DescriptorSetLayout<> descriptorSetLayout;
	magma::PipelineLayout<> pipelineLayout;
	magma::ShaderModule<> vert;
	magma::ShaderModule<> frag;

	UserData(magma::Device<claws::no_delete> device, vk::PhysicalDevice, uint32_t selectedQueueFamily)
	  : commandPool(device.createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, selectedQueueFamily))
	  , descriptorSetLayout(device.createDescriptorSetLayout({
		vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr
		    }}))
	  , pipelineLayout(device.createPipelineLayout({}, {descriptorSetLayout}, {}))
	{
	  {
	    std::ifstream vertSource("spirv/basic.vert.spirv");
	    std::ifstream fragSource("spirv/basic.frag.spirv");

	    if (!vertSource || !fragSource)
	      throw std::runtime_error("Failed to load shaders");
	    vert = device.createShaderModule(static_cast<std::istream &>(vertSource));
	    frag = device.createShaderModule(static_cast<std::istream &>(fragSource));
	  }
	}

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
	magma::Pipeline<> createPipeline(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData const &userData);

	SwapchainUserData() = default;
	SwapchainUserData(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData &userData, uint32_t imageCount)
	  : commandBuffers(userData.commandPool.allocatePrimaryCommandBuffers(imageCount))
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

	      vk::AttachmentReference colorAttachmentReferences(0, vk::ImageLayout::eColorAttachmentOptimal);

	      renderPassCreateInfo.subPasses.push_back(magma::StructBuilder<vk::SubpassDescription, true>
						       ::make(vk::PipelineBindPoint::eGraphics,
							      magma::EmptyList{},
							      magma::asListRef(colorAttachmentReferences),
							      nullptr,
							      nullptr,
							      magma::EmptyList{}));

	      return device.createRenderPass(renderPassCreateInfo);
	    }())
	  , pipeline(createPipeline(device, swapchain, userData))
	{
	}
      };

      struct FrameData
      {
	magma::Fence<> fence;
	magma::Image<> depthImage;
	magma::DeviceMemory<> depthImageMemory;
	magma::ImageView<> depthImageView;
	magma::Framebuffer<> framebuffer;
	magma::DynamicBuffer::RangeId vertexBufferRangeId{magma::DynamicBuffer::nullId};
	magma::DynamicBuffer::RangeId indexBufferRangeId{magma::DynamicBuffer::nullId};

	FrameData(magma::Device<claws::no_delete> device,
		  magma::Swapchain<claws::no_delete> swapchain,
		  UserData &,
		  SwapchainUserData &swapchainUserData,
		  magma::ImageView<claws::no_delete> swapchainImageView)
	  : fence(device.createFence(vk::FenceCreateFlagBits::eSignaled))
	  , depthImageMemory{}
	  , depthImage([&]()
		       {
			 auto image(device.createImage2D({}, vk::Format::eR8G8B8A8Unorm, {swapchain.getExtent().width, swapchain.getExtent().height}, vk::SampleCountFlagBits::e1,
							 vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageLayout::eUndefined));
			 auto memRequirements(device.getImageMemoryRequirements(depthImage));
			 depthImageMemory = device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits);

			 device.bindImageMemory(image, depthImageMemory, 0);
			 return image;
		       }())
	  , framebuffer(device.createFramebuffer(swapchainUserData.renderPass,
						 std::vector<vk::ImageView>{swapchainImageView},
						 swapchain.getExtent().width,
						 swapchain.getExtent().height,
						 1))
	{
	}
      };
      vk::PhysicalDevice physicalDevice;
      magma::Device<> device;
      magma::Semaphore<> imageAvailable;
      magma::Semaphore<> renderDone;
      vk::Queue queue;
      magma::DisplaySystem<UserData, SwapchainUserData, FrameData> displaySystem;

      magma::DescriptorPool<> descriptorPool;
      magma::DescriptorSets<> descriptorSets;
      magma::Image<> backgroundImage;
      magma::DeviceMemory<> backgroundImageMemory;
      magma::ImageView<> backgroundImageView;
      magma::DynamicBuffer stagingBuffer;
      magma::DynamicBuffer vertexBuffer;
      magma::Sampler<> sampler;

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

      Renderer(std::pair<vk::PhysicalDevice, Score> const &selectedResult, magma::Surface<claws::no_delete> surface)
	: physicalDevice(selectedResult.first)
	, device([this, &selectedResult, surface](){
	    float priority{1.0f};
	    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, selectedResult.second.bestQueue, 1, &priority};

	    return magma::Device<>(physicalDevice,
				   std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
				   std::vector<char const *>({VK_KHR_SWAPCHAIN_EXTENSION_NAME}));
	  }())
	, imageAvailable(device.createSemaphore())
	, renderDone(device.createSemaphore())
	, queue(device.getQueue(selectedResult.second.bestQueue, 0u))
	, displaySystem(physicalDevice, surface, device, queue, selectedResult.second.bestQueue)
	, descriptorPool(device.createDescriptorPool(1, {vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1}}))
	, descriptorSets(descriptorPool.allocateDescriptorSets({displaySystem.userData.descriptorSetLayout}))
	, backgroundImage(device.createImage2D({}, vk::Format::eR8G8B8A8Unorm, {display::superCorbeau::width, display::superCorbeau::height}, vk::SampleCountFlagBits::e1,
					       vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageLayout::eUndefined))
	, backgroundImageMemory([this](){
	    auto memRequirements(device.getImageMemoryRequirements(backgroundImage));

	    auto memory(device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits));
	    device.bindImageMemory(backgroundImage, memory, 0);
	    return memory;
	  }())
	, backgroundImageView(device.createImageView({},
						     backgroundImage,
						     vk::ImageViewType::e2D,
						     vk::Format::eR8G8B8A8Unorm,
						     vk::ComponentMapping{},
						     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
									       0,
									       1,
									       0,
									       1)))
	, stagingBuffer(device,
			physicalDevice,
			{},
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible,
			std::vector<uint32_t>{selectedResult.second.bestQueue})
	, vertexBuffer(device,
		       physicalDevice,
		       {},
		       vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
		       vk::MemoryPropertyFlagBits::eHostVisible,
		       std::vector<uint32_t>{selectedResult.second.bestQueue})
	, sampler(device.createSampler(vk::Filter::eNearest,
				       vk::Filter::eNearest,
				       vk::SamplerMipmapMode::eNearest,
				       vk::SamplerAddressMode::eClampToEdge,
				       vk::SamplerAddressMode::eClampToEdge,
				       vk::SamplerAddressMode::eClampToEdge,
				       0.0f,
				       false,
				       0.0f, // no effect
				       false,
				       vk::CompareOp::eAlways, // no effect
				       0.0f,
				       0.0f,
				       vk::BorderColor::eIntOpaqueWhite,
				       false))
      {
	{
	  magma::DynamicBuffer::RangeId tmpBuffer(stagingBuffer.allocate(display::superCorbeau::width * display::superCorbeau::height * 4));
	  auto memory(stagingBuffer.getMemory<unsigned char []>(tmpBuffer));
	  auto src(display::superCorbeau::header_data);
	  for (unsigned int i(0u); i < display::superCorbeau::width * display::superCorbeau::height; ++i)
	    display::superCorbeau::headerPixel(src, &memory[i * 4]);
	  auto commandBuffers(displaySystem.userData.commandPool.allocatePrimaryCommandBuffers(1));
	  commandBuffers[0].begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	  vk::ImageSubresourceRange imageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

	  commandBuffers[0].raw().pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {},
						  {
						    vk::ImageMemoryBarrier{
						      {},
							vk::AccessFlagBits::eTransferWrite,
							  vk::ImageLayout::eUndefined,
							  vk::ImageLayout::eTransferDstOptimal,
							  VK_QUEUE_FAMILY_IGNORED,
							  VK_QUEUE_FAMILY_IGNORED,
							  backgroundImage,
							  imageSubresourceRange
							  }
						  });
	  commandBuffers[0].raw().copyBufferToImage(stagingBuffer.getBuffer(tmpBuffer),
						    backgroundImage,
						    vk::ImageLayout::eTransferDstOptimal,
						    {
						      vk::BufferImageCopy{
							tmpBuffer.second,
							  0, 0,
							  vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
							  vk::Offset3D{0, 0, 0},
							    vk::Extent3D{display::superCorbeau::width, display::superCorbeau::height, 1}
						      }
						    });
	  ;
	  commandBuffers[0].raw().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {},
						  {
						    vk::ImageMemoryBarrier{
						      vk::AccessFlagBits::eTransferWrite,
							vk::AccessFlagBits::eShaderRead,
							vk::ImageLayout::eTransferDstOptimal,
							vk::ImageLayout::eShaderReadOnlyOptimal,
							VK_QUEUE_FAMILY_IGNORED,
							VK_QUEUE_FAMILY_IGNORED,
							backgroundImage,
							imageSubresourceRange
							}
						  });

	  commandBuffers[0].end();
	  magma::Fence<> fence(device.createFence({}));
	  queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::EmptyList(),
								  nullptr,
								  magma::asListRef(commandBuffers[0].raw()),
								  magma::EmptyList()),
		       fence);
	  device.waitForFences({fence}, true, 1000000000);
	}

	vk::DescriptorImageInfo const imageInfo
	{
	  sampler,
	    backgroundImageView,
	    vk::ImageLayout::eShaderReadOnlyOptimal
	    };
	device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1u>{vk::WriteDescriptorSet{descriptorSets[0], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr}});
      }

    public:
      Renderer(magma::Instance const &instance, magma::Surface<claws::no_delete> surface)
	: Renderer([&instance, surface](){
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

      ~Renderer() = default;

      void render(WindowTree const &windowTree);
      uint32_t prepareGpuData(FrameData &frame, WindowTree const &windowTree);
    };

    magma::Instance instance;
    magma::Surface<> surface;
    Renderer renderer;

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

    void render(WindowTree const &windowTree)
    {
      renderer.render(windowTree);
    }
  };
}
