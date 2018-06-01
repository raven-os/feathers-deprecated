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

#include "display/SuperCorbeau.hpp"

namespace display
{
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
	  , descriptorSetLayout(device.createDescriptorSetLayout({vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr}}))
	  , pipelineLayout(device.createPipelineLayout({}, {descriptorSetLayout}, {vk::PushConstantRange{vk::ShaderStageFlagBits::eVertex, 0, sizeof(float) * 4u}}))
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
      };

      struct SwapchainUserData
      {
	magma::CommandBufferGroup<magma::PrimaryCommandBuffer> commandBuffers;
	magma::RenderPass<> renderPass;
	magma::Pipeline<> pipeline;

	magma::Pipeline<> createPipeline(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData const &userData)
	{
	  std::cout << "creating pipeline for swapchain with extent " << swapchain.getExtent().width << ", " << swapchain.getExtent().height << std::endl;
	  std::array<vk::SpecializationMapEntry, 2u> mapEntries{
	    vk::SpecializationMapEntry{0, 0, sizeof(float)},
	      vk::SpecializationMapEntry{1, sizeof(float), sizeof(float)}
	  };
	  std::array<float, 2u> dimensions{static_cast<float>(swapchain.getExtent().width), static_cast<float>(swapchain.getExtent().height)};

	  auto specialzationInfo(magma::StructBuilder<vk::SpecializationInfo>::make(mapEntries, sizeof(float) * 2, dimensions.data()));

	  std::vector<vk::PipelineShaderStageCreateInfo>
	    shaderStageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, userData.vert, "main", &specialzationInfo},
	      {{}, vk::ShaderStageFlagBits::eFragment, userData.frag, "main", nullptr}};

	  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleFan, false};
	   std::array<vk::VertexInputBindingDescription, 1u> vertexInputBindings{
	     vk::VertexInputBindingDescription{0, 6 * sizeof(float), vk::VertexInputRate::eVertex}};
	   std::array<vk::VertexInputAttributeDescription, 2u>
	     vertexInputAttrib{vk::VertexInputAttributeDescription{0, vertexInputBindings[0].binding, vk::Format::eR32G32Sfloat, 0},
	       vk::VertexInputAttributeDescription{1, vertexInputBindings[0].binding, vk::Format::eR32G32B32A32Sfloat, 2 * sizeof(float)}};

	  auto vertexInputStateCreateInfo(magma::StructBuilder<vk::PipelineVertexInputStateCreateInfo, true>::make(vertexInputBindings,
														   vertexInputAttrib));

	  vk::Viewport viewport(0.0f,
				0.0f, // pos
				static_cast<float>(swapchain.getExtent().width),
				static_cast<float>(swapchain.getExtent().height), // size
				0.0f,
				1.0); // depth range

	  vk::Rect2D scissor({0, 0}, swapchain.getExtent());

	  auto viewportStateCreateInfo(magma::StructBuilder<vk::PipelineViewportStateCreateInfo, true>::make(magma::asListRef(viewport),
													     magma::asListRef(scissor)));

	  vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
	    {},
	      false,                            // VkBool32                                       depthClampEnable
		false,                            // VkBool32                                       rasterizerDiscardEnable
		vk::PolygonMode::eFill,           // VkPolygonMode                                  polygonMode
		vk::CullModeFlagBits::eBack,      // VkCullModeFlags                                cullMode
		vk::FrontFace::eCounterClockwise, // VkFrontFace                                    frontFace
		false,                            // VkBool32                                       depthBiasEnable
		0.0f,                             // float                                          depthBiasConstantFactor
		0.0f,                             // float                                          depthBiasClamp
		0.0f,                             // float                                          depthBiasSlopeFactor
		1.0f                              // float                                          lineWidth
		};

	  vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
	    {},
	      vk::SampleCountFlagBits::e1, // VkSampleCountFlagBits                          rasterizationSamples
		false,                       // VkBool32                                       sampleShadingEnable
		1.0f,                        // float                                          minSampleShading
		nullptr,                     // const VkSampleMask                            *pSampleMask
		false,                       // VkBool32                                       alphaToCoverageEnable
		false                        // VkBool32                                       alphaToOneEnable
		};

	  vk::PipelineColorBlendAttachmentState
	    colorBlendAttachmentState{false,                  // VkBool32                                       blendEnable
	      vk::BlendFactor::eOne,  // VkBlendFactor                                  srcColorBlendFactor
	      vk::BlendFactor::eZero, // VkBlendFactor                                  dstColorBlendFactor
	      vk::BlendOp::eAdd,      // VkBlendOp                                      colorBlendOp
	      vk::BlendFactor::eOne,  // VkBlendFactor                                  srcAlphaBlendFactor
	      vk::BlendFactor::eZero, // VkBlendFactor                                  dstAlphaBlendFactor
	      vk::BlendOp::eAdd,      // VkBlendOp                                      alphaBlendOp
	      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB
	      | vk::ColorComponentFlagBits::eA};

	  vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
	    {},                 // VkPipelineColorBlendStateCreateFlags           flags
	      false,              // VkBool32                                       logicOpEnable
		vk::LogicOp::eCopy, // VkLogicOp                                      logicOp
		1,
		&colorBlendAttachmentState, // const VkPipelineColorBlendAttachmentState     *pAttachments
		  {0.0f, 0.0f, 0.0f, 0.0f}    // float                                          blendConstants[4]
	  };

	  magma::GraphicsPipelineConfig pipelineCreateInfo({},
							   std::move(shaderStageCreateInfos),
							   vertexInputStateCreateInfo,
							   inputAssemblyStateCreateInfo,
							   rasterizationStateCreateInfo,
							   userData.pipelineLayout,
							   renderPass,
							   0);

	  pipelineCreateInfo.addRasteringColorAttachementInfo(viewportStateCreateInfo, multisampleStateCreateInfo, colorBlendStateCreateInfo);
	  return device.createPipeline(pipelineCreateInfo);
	}

	SwapchainUserData() = default;
	SwapchainUserData(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData &userData, uint32_t imageCount)
	  : commandBuffers(userData.commandPool.allocatePrimaryCommandBuffers(imageCount))
	  , renderPass([&](){
	      magma::RenderPassCreateInfo renderPassCreateInfo{{}};

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
	  , pipeline(std::move(createPipeline(device, swapchain, userData)))
	{
	}
      };

      struct FrameData
      {
	magma::Fence<> fence;
	magma::Framebuffer<> framebuffer;

	FrameData(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData &, SwapchainUserData &swapchainUserData, magma::ImageView<claws::no_delete> swapchainImageView)
	  : fence(device.createFence(vk::FenceCreateFlagBits::eSignaled))
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

      magma::Buffer<> quadBuffer;
      magma::DeviceMemory<> quadBufferMemory;
      magma::DescriptorPool<> descriptorPool;
      magma::DescriptorSets<> descriptorSets;
      magma::Image<> backgroundImage;
      magma::DeviceMemory<> backgroundImageMemory;
      magma::ImageView<> backgroundImageView;
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
	    float priority[]{ 1.0 };
	    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{{}, selectedResult.second.bestQueue, 1, priority};

	    return magma::Device<>(physicalDevice,
				   std::vector<vk::DeviceQueueCreateInfo>({deviceQueueCreateInfo}),
				   std::vector<char const *>({VK_KHR_SWAPCHAIN_EXTENSION_NAME}));
	  }())
	, imageAvailable(device.createSemaphore())
	, renderDone(device.createSemaphore())
	, queue(device.getQueue(selectedResult.second.bestQueue, 0u))
	, displaySystem(physicalDevice, surface, device, queue, selectedResult.second.bestQueue)
	, quadBuffer(device.createBuffer({}, 4 * sizeof(float), vk::BufferUsageFlagBits::eVertexBuffer, {selectedResult.second.bestQueue}))
	, quadBufferMemory([this](){
	    auto memRequirements(device.getBufferMemoryRequirements(quadBuffer));

	    return device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits);
	  }())
	, descriptorPool(device.createDescriptorPool(1, {vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1}}))
	, descriptorSets(descriptorPool.allocateDescriptorSets({displaySystem.userData.descriptorSetLayout}))
	, backgroundImage(device.createImage2D({}, vk::Format::eR8G8B8Unorm, {display::superCorbeau::width, display::superCorbeau::height}, vk::SampleCountFlagBits::e1,
					       vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eSampled, vk::ImageLayout::ePreinitialized))
	, backgroundImageMemory([this](){
	    auto memRequirements(device.getImageMemoryRequirements(backgroundImage));
	    auto result(device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible, memRequirements.memoryTypeBits));
	    
	    device.bindImageMemory(backgroundImage, backgroundImageMemory, 0);
	    return result;
	  }())
	, backgroundImageView(device.createImageView({},
						     backgroundImage,
						     vk::ImageViewType::e2D,
						     vk::Format::eR8G8B8Unorm,
						     vk::ComponentMapping{},
						     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
									       0,
									       VK_REMAINING_MIP_LEVELS,
									       0,
									       VK_REMAINING_ARRAY_LAYERS
									       )))
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
				       1.0f,
				       vk::BorderColor::eIntOpaqueBlack,
				       false))
      {
	device.bindBufferMemory(quadBuffer, quadBufferMemory, 0);
	{
	  auto deleter([&](auto data) {
	      if (data)
		device.unmapMemory(backgroundImageMemory);
	    });
	  std::unique_ptr<unsigned char[], decltype(deleter)> imageData(reinterpret_cast<unsigned char *>(device.mapMemory(backgroundImageMemory, 0, VK_WHOLE_SIZE)),
									deleter);
	  auto src(display::superCorbeau::header_data);
	  for (unsigned int i(0u); i < display::superCorbeau::width * display::superCorbeau::height; ++i)
	    {
	      std::array<unsigned char, 4> pixel;
	      display::superCorbeau::headerPixel(src, pixel.data());
	      for (unsigned int j(0u); j < 3u; ++j) {
		imageData[3 * i + j] = pixel[j];
	      }
	    }
	}
	vk::DescriptorImageInfo const imageInfo
	{
	  sampler,
	    backgroundImageView,
	    vk::ImageLayout::eColorAttachmentOptimal
	};
	device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1u>{vk::WriteDescriptorSet{descriptorSets[0], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr}});
	{
	  auto deleter([&](auto data) {
	      if (data)
		device.unmapMemory(quadBufferMemory);
	    });
	  std::unique_ptr<float[], decltype(deleter)> quadData(reinterpret_cast<float *>(device.mapMemory(quadBufferMemory, 0, VK_WHOLE_SIZE)),
							       deleter);
	  quadData[0] = 0.0f;
	  quadData[1] = 0.0f;
	  quadData[2] = 1.0f;
	  quadData[3] = 0.0f;
	  quadData[4] = 0.0f;
	  quadData[5] = 1.0f;
	  quadData[6] = 1.0f;
	  quadData[7] = 1.0f;
	}
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

      ~Renderer() noexcept
      {
	try {
	  quadBuffer = magma::Buffer<>{}; // destroy buffer before memory being free'd
	  backgroundImage = magma::Image<>{}; // destroy image before memory being free'd
	} catch (...) {
	  std::cerr << "swallowing error: failed to destroy quadBuffer\n" << std::endl;
	}
      }

      void render()
      {
	auto [index, frame] = displaySystem.getImage(imageAvailable);

	device.waitForFences({frame.fence}, true, 1000000000);
	device.resetFences({frame.fence});
	magma::PrimaryCommandBuffer cmdBuffer(displaySystem.swapchainUserData.commandBuffers[index]);
	uint32_t vertexCount(4u);

	cmdBuffer.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmdBuffer.bindVertexBuffers(0, {quadBuffer}, {0ul});
	cmdBuffer.bindVertexBuffers(1, {quadBuffer}, {0ul});
	cmdBuffer.raw().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, displaySystem.userData.pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
	vk::ClearValue clearValue = {vk::ClearColorValue(std::array<float, 4>{0.01f, 0.02f, 0.1f, 1.0f})};
	{
	  auto lock(cmdBuffer.beginRenderPass(displaySystem.swapchainUserData.renderPass, frame.framebuffer,
					      {{0, 0}, displaySystem.getSwapchain().getExtent()}, {clearValue}, vk::SubpassContents::eInline));
	  
	  lock.bindGraphicsPipeline(displaySystem.swapchainUserData.pipeline);
	  lock.draw(vertexCount, 1, 0, 0);
	}
	cmdBuffer.end();

	vk::PipelineStageFlags waitDestStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(asListRef(imageAvailable),
								&waitDestStageMask,
								magma::asListRef(cmdBuffer.raw()),
								magma::asListRef(renderDone)),
		     frame.fence);
	device.waitIdle();
	std::cout << "about to present for index " << index << std::endl;
	displaySystem.presentImage(renderDone, index);
      }
    };

    magma::Instance instance;
    magma::Surface<> surface;
    Renderer renderer;

  public:
    template<class SurfaceProvider>
    Display(SurfaceProvider &surfaceProvider)
      : instance{SurfaceProvider::getRequiredExtensiosn()}
      , surface(surfaceProvider.createSurface(instance))
      , renderer(instance, surface)
    {
    }

    Display(Display const &) = delete;
    Display(Display &&) = delete;
    Display operator=(Display const &) = delete;
    Display operator=(Display &&) = delete;

    void render()
    {
      renderer.render();
    }
  };
}
