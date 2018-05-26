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

namespace display
{
  template<class SurfaceProvider>
  class Display
  {
    magma::Instance instance;
    SurfaceProvider surfaceProvider;

    struct Renderer
    {
      struct UserData
      {
	magma::CommandPool<> commandPool;
	magma::PipelineLayout<> pipelineLayout;
	magma::ShaderModule<> vert;
	magma::ShaderModule<> frag;

	UserData(magma::Device<claws::no_delete> device, vk::PhysicalDevice, uint32_t selectedQueueFamily)
	  : commandPool(device.createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, selectedQueueFamily))
	  , pipelineLayout(device.createPipelineLayout({}, {}, {}))
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
	  std::vector<vk::PipelineShaderStageCreateInfo>
	    shaderStageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, userData.vert, "main", nullptr},
	      {{}, vk::ShaderStageFlagBits::eFragment, userData.frag, "main", nullptr}};

	  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleList, false};

	  auto vertexInputStateCreateInfo(magma::StructBuilder<vk::PipelineVertexInputStateCreateInfo, true>::make(magma::EmptyList(),
														   magma::EmptyList()));

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

	FrameData(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete>, UserData &, SwapchainUserData &, magma::ImageView<claws::no_delete>)
	  : fence(device.createFence(vk::FenceCreateFlagBits::eSignaled))
	{
	}
      };
      vk::PhysicalDevice physicalDevice;
      magma::Device<> device;
      magma::Semaphore<> imageAvailable;
      magma::Semaphore<> renderDone;
      vk::Queue queue;
      magma::DisplaySystem<UserData, SwapchainUserData, FrameData> displaySystem;

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
      {
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

      void render()
      {
	auto [index, frame] = displaySystem.getImage(imageAvailable);

	device.waitForFences({frame.fence}, true, 1000000000);
	device.resetFences({frame.fence});
	// {
	// 	std::lock_guard<std::mutex> logicLock(lock);
	// 	recordCommandBuffer(index, frame, logic);
	// }
	magma::PrimaryCommandBuffer cmdBuffer(displaySystem.swapchainUserData.commandBuffers[index]);

	cmdBuffer.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	//	cmdBuffer.
	// cmdBuffer.clearColorImage(frame.imageView);
      
	cmdBuffer.end();

	vk::PipelineStageFlags waitDestStageMask(0// vk::PipelineStageFlagBits::eColorAttachmentOutput
						   );
	queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::EmptyList(), // asListRef(imageAvailable),
								//							        &waitDestStageMask,
								nullptr,
								magma::asListRef(cmdBuffer.raw()),
								magma::asListRef(renderDone)),
			      frame.fence);
	displaySystem.presentImage(renderDone, index);
      }
    };

    Renderer renderer;

  public:

    Display()
      : instance{SurfaceProvider::getRequiredExtensiosn()}
      , surfaceProvider{instance}
      , renderer(instance, surfaceProvider.getSurface())
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
