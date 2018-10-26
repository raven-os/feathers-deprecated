#include "display/Renderer.hpp"
#include "wm/WindowTree.hpp"

#include <claws/utils/lambda_utils.hpp>

namespace display
{
  Renderer::Renderer(magma::Device<claws::no_delete> device, vk::PhysicalDevice physicalDevice, uint32_t selectedQueueFamily)
    : commandPool(device.createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, selectedQueueFamily))
    , descriptorSetLayout(device.createDescriptorSetLayout({
	  vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr
	      },
	    vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eSampledImage, 1, vk::ShaderStageFlagBits::eFragment, nullptr
		}}))
    , pipelineLayout(device.createPipelineLayout({}, {descriptorSetLayout}, {}))
    , physicalDevice(physicalDevice)
    , renderDone(device.createSemaphore())
    , queue(device.getQueue(selectedQueueFamily, 0u))
    , descriptorPool(device.createDescriptorPool(1, {vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 1}, vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1}}))
    , descriptorSets(descriptorPool.allocateDescriptorSets({descriptorSetLayout}))
    , backgroundImageMemory()
    , backgroundImage([&](){
	auto image(device.createImage2D({}, vk::Format::eR8G8B8A8Unorm, {display::superCorbeau::width, display::superCorbeau::height}, vk::SampleCountFlagBits::e1,
					vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageLayout::eUndefined));

	auto memRequirements(device.getImageMemoryRequirements(image));

	this->backgroundImageMemory = device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eDeviceLocal, memRequirements.memoryTypeBits);
	device.bindImageMemory(image, backgroundImageMemory, 0);
	return image;
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
		    vk::MemoryPropertyFlagBits::eHostVisible)
    , vertexBuffer(device,
		   physicalDevice,
		   {},
		   vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
		   vk::MemoryPropertyFlagBits::eHostVisible)
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
      {
	magma::DynamicBuffer::RangeId tmpBuffer(stagingBuffer.allocate(display::superCorbeau::width * display::superCorbeau::height * 4));
	auto memory(stagingBuffer.getMemory<unsigned char []>(tmpBuffer));
	auto src(display::superCorbeau::header_data);
	for (unsigned int i(0u); i < display::superCorbeau::width * display::superCorbeau::height; ++i)
	  display::superCorbeau::headerPixel(src, &memory[i * 4]);
	auto commandBuffers(commandPool.allocatePrimaryCommandBuffers(1));
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

      {
	vk::DescriptorImageInfo const imageInfo
	{
	  sampler,
	    nullptr,
	    vk::ImageLayout::eShaderReadOnlyOptimal
	    };
	device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1u>{vk::WriteDescriptorSet{descriptorSets[0], 0, 0, 1, vk::DescriptorType::eSampler, &imageInfo, nullptr, nullptr}});
      }
      {
	vk::DescriptorImageInfo const imageInfo
	{
	  nullptr,
	    backgroundImageView,
	    vk::ImageLayout::eShaderReadOnlyOptimal
	    };
	device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1u>{vk::WriteDescriptorSet{descriptorSets[0], 1, 0, 1, vk::DescriptorType::eSampledImage, &imageInfo, nullptr, nullptr}});
      }
    }
    {
      std::ifstream vertSource("spirv/basic.vert.spirv");
      std::ifstream fragSource("spirv/basic.frag.spirv");

      if (!vertSource || !fragSource)
	throw std::runtime_error("Failed to load shaders");
      vert = device.createShaderModule(static_cast<std::istream &>(vertSource));
      frag = device.createShaderModule(static_cast<std::istream &>(fragSource));
    }
  }

  uint32_t Renderer::prepareGpuData(FrameData &frame, WindowTree const &windowTree)
  {
    std::vector<float> vertexData{};
    std::vector<uint32_t> indexData{};

    vertexData.reserve(windowTree.getWindowCountUpperBound() * 5 * 4);
    indexData.reserve(windowTree.getWindowCountUpperBound() * 6);
    auto vertexDataIt(std::back_inserter(vertexData));
    auto indexDataIt(std::back_inserter(indexData));
    uint32_t windowCount(0u);
    auto display([this, &windowTree, &vertexDataIt, &indexDataIt, &windowCount](auto display, WindowTree::WindowNodeIndex index, float minDepth, float range) -> void
		 {
		   std::vector<WindowTree::WindowNodeIndex> delayedRender;
		   for (WindowTree::WindowNodeIndex child : windowTree.getChildren(index))
		     if (windowTree.getData(child).isSolid)
		       display(child, minDepth + range * 0.5f, range * 0.4f);
		     else
		       delayedRender.push_back(child);
		   if (windowTree.getData(index).isVisible())
		     {
		       Rect const &rect(windowTree.getData(index).rect);
		       for (uint32_t i(0u); i < 4u; ++i)
			 {
			   for (uint32_t j(0u); j < 2u; ++j)
			     *vertexDataIt++ = float(rect.position[j] + rect.size[j] * ((i >> j) & 1u));
			   *vertexDataIt++ =  minDepth + range;
			   for (uint32_t j(0u); j < 2u; ++j)
			     *vertexDataIt++ = float((i >> j) & 1u);
			 }
		       *indexDataIt++ = windowCount * 4 + 0;
		       *indexDataIt++ = windowCount * 4 + 1;
		       *indexDataIt++ = windowCount * 4 + 2;
		       *indexDataIt++ = windowCount * 4 + 1;
		       *indexDataIt++ = windowCount * 4 + 2;
		       *indexDataIt++ = windowCount * 4 + 3;
		       ++windowCount;
		     }
		   float childRange(range * 0.5f / float(delayedRender.size()));
		   float depth(minDepth);
		   for (WindowTree::WindowNodeIndex child : delayedRender)
		     {
		       display(child, depth, depth + childRange);
		       depth += childRange;
		     }
		 });
    claws::inject_self{display}(windowTree.getRootIndex(), 0.0f, 0.9f);
    vertexBuffer.free(frame.vertexBufferRangeId);
    vertexBuffer.free(frame.indexBufferRangeId);
    if (!windowCount)
      {
	frame.vertexBufferRangeId = magma::DynamicBuffer::nullId;
	frame.indexBufferRangeId = magma::DynamicBuffer::nullId;
	return windowCount;
      }
    frame.vertexBufferRangeId = vertexBuffer.allocate(static_cast<uint32_t>(vertexData.size() * sizeof(float)));
    frame.indexBufferRangeId = vertexBuffer.allocate(static_cast<uint32_t>(indexData.size() * sizeof(uint32_t)));
    {
      auto gpuBufferRange(vertexBuffer.getMemory<float[]>(frame.vertexBufferRangeId));

      std::copy(vertexData.begin(), vertexData.end(), &gpuBufferRange[0]);
    }
    {
      auto gpuBufferRange(vertexBuffer.getMemory<uint32_t[]>(frame.indexBufferRangeId));

      std::copy(indexData.begin(), indexData.end(), &gpuBufferRange[0]);
    }
    return windowCount;
  }



  magma::Semaphore<claws::no_delete> Renderer::render(magma::Device<claws::no_delete> device, WindowTree const &windowTree, unsigned int index, SwapchainUserData &swapchainUserData, FrameData &frame, magma::Semaphore<claws::no_delete> imageAvailable)
  {
    // wait for rendering fence
    device.waitForFences({frame.fence}, true, 1000000000);
    // reset fence
    device.resetFences({frame.fence});
    uint32_t const vertexCount(prepareGpuData(frame, windowTree) * 6);
    magma::PrimaryCommandBuffer cmdBuffer(swapchainUserData.commandBuffers[index]);

    // being command recording
    cmdBuffer.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    if (vertexCount)
      {
	// we bind our vertex buffer range
	cmdBuffer.bindVertexBuffers(0, {vertexBuffer.getBuffer(frame.vertexBufferRangeId)}, {frame.vertexBufferRangeId.second});
	// we bind our index buffer range
	cmdBuffer.bindIndexBuffer(vertexBuffer.getBuffer(frame.indexBufferRangeId), frame.indexBufferRangeId.second, vk::IndexType::eUint32);
	// we bind update our descriptor so that it points to our image
	cmdBuffer.raw().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
      }
    std::vector<vk::ClearValue> clearValues{
      vk::ClearColorValue(std::array<float, 4>{0.0f, 0.5f, 0.0f, 1.0f}),
	vk::ClearDepthStencilValue(1.0f, 0),
	}; // a nice recognisable green for debug
    {
      // start the renderpass
      auto lock(cmdBuffer.beginRenderPass(swapchainUserData.renderPass, frame.framebuffer,
					  {{0, 0}, getExtent()}, clearValues, vk::SubpassContents::eInline));

      // us our pipeline
      lock.bindGraphicsPipeline(swapchainUserData.pipeline);

      
      // draw our quad
      if (vertexCount)
	lock.drawIndexed(vertexCount, 1, 0, 0, 0);
    }
    cmdBuffer.end();

    vk::PipelineStageFlags waitDestStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::asListRef(imageAvailable), // wait fo image to be available
							    &waitDestStageMask,
							    magma::asListRef(cmdBuffer.raw()),
							    magma::asListRef(renderDone)), // signal renderdone when done
		 frame.fence); // signal the fence
    return renderDone;
  }



  magma::Pipeline<> SwapchainUserData::createPipeline(magma::Device<claws::no_delete> device, vk::Extent2D extent, Renderer const &renderer)
  {
    std::cout << "creating pipeline for swapchain with extent " << extent.width << ", " << extent.height << std::endl;
    /// --- Specialisation info --- ///
    // The width and height are specialized rather than being push constants, because a compositor shoudn't change size often (If ever).
    // Both entries have the size of a float
    // The second entry is offset by one float
    std::array<vk::SpecializationMapEntry, 2u> mapEntries{
      vk::SpecializationMapEntry{0, 0, sizeof(float)},
	vk::SpecializationMapEntry{1, sizeof(float), sizeof(float)}
    };
    // These are the actual specialisation values
    std::array<float, 2u> dimensions{static_cast<float>(extent.width), static_cast<float>(extent.height)};

    auto specialzationInfo(magma::StructBuilder<vk::SpecializationInfo>::make(mapEntries, sizeof(float) * dimensions.size(), dimensions.data()));
    /// --- Specialisation info END --- ///

    // We have two shaders, and both shader's entrypoints are "main".
    // Only the vertex shader is specialized (see above setup)
    std::vector<vk::PipelineShaderStageCreateInfo>
      shaderStageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, renderer.vert, "main", &specialzationInfo},
	{{}, vk::ShaderStageFlagBits::eFragment, renderer.frag, "main", nullptr}};

    // We are rendering triangle strips, and primitives shouldn't restart.
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleList, false};
    // We have one vertex buffers:  position and coords are one after eachother
    std::array<vk::VertexInputBindingDescription, 1u> vertexInputBindings{
      vk::VertexInputBindingDescription{0, 5 * sizeof(float), vk::VertexInputRate::eVertex}
    };
    // Both vertex buffers contains `vec2`s of floats, which is the format `vk::Format::eR32G32Sfloat`
    std::array<vk::VertexInputAttributeDescription, 2u>
      vertexInputAttrib{vk::VertexInputAttributeDescription{0, vertexInputBindings[0].binding, vk::Format::eR32G32B32Sfloat, 0},
	vk::VertexInputAttributeDescription{1, vertexInputBindings[0].binding, vk::Format::eR32G32Sfloat, 3 * sizeof(float)}};

    auto vertexInputStateCreateInfo(magma::StructBuilder<vk::PipelineVertexInputStateCreateInfo, true>::make(vertexInputBindings,
													     vertexInputAttrib));

    // The viewport is st up so that the top left corner is (0, 0) and the bottom right (width, height)
    vk::Viewport viewport(-static_cast<float>(extent.width),
			  -static_cast<float>(extent.height), // pos
			  static_cast<float>(extent.width) * 2.0f,
			  static_cast<float>(extent.height) * 2.0f, // size
			  0.0f,
			  1.0); // depth range

    // Scissors cover all that is within the compositor
    vk::Rect2D scissor({0, 0}, extent);

    auto viewportStateCreateInfo(magma::StructBuilder<vk::PipelineViewportStateCreateInfo, true>::make(magma::asListRef(viewport),
												       magma::asListRef(scissor)));

    // We want to benefit from earl depth test
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
      {},
	false,                            // VkBool32                                       depthClampEnable
	  false,                            // VkBool32                                       rasterizerDiscardEnable
	  vk::PolygonMode::eFill,           // VkPolygonMode                                  polygonMode
	  vk::CullModeFlagBits::eNone,      // VkCullModeFlags                                cullMode
	  vk::FrontFace::eCounterClockwise, // VkFrontFace                                    frontFace
	  false,                            // VkBool32                                       depthBiasEnable
	  0.0f,                             // float                                          depthBiasConstantFactor
	  0.0f,                             // float                                          depthBiasClamp
	  0.0f,                             // float                                          depthBiasSlopeFactor
	  1.0f                              // float                                          lineWidth
	  };

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo
      (
       {},			// VkPipelineDepthStencilStateCreateFlags    flags;
       true,			// VkBool32                                  depthTestEnable;
       true,			// VkBool32                                  depthWriteEnable;
       vk::CompareOp::eLess,	// VkCompareOp                               depthCompareOp;
       false,			// VkBool32                                  depthBoundsTestEnable;
       false,			// VkBool32                                  stencilTestEnable;
       vk::StencilOp::eKeep,	// VkStencilOpState                          front;
       vk::StencilOp::eKeep,	// VkStencilOpState                          back;
       0.0f,			// float                                     minDepthBounds;
       1.0f			// float                                     maxDepthBounds;
       );

    // Everything is turned off
    // We use only one smape per pixel
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
      {},
	vk::SampleCountFlagBits::e1, // VkSampleCountFlagBits                          rasterizationSamples
	  false,                       // VkBool32                                       sampleShadingEnable
	  1.0f,                        // float                                          minSampleShading
	  nullptr,                     // const VkSampleMask                            *pSampleMask
	  false,                       // VkBool32                                       alphaToCoverageEnable
	  false                        // VkBool32                                       alphaToOneEnable
	  };

    // No blending
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
						     renderer.pipelineLayout,
						     renderPass,
						     0);

    pipelineCreateInfo.addRasteringColorAttachementDepthStencilInfo(viewportStateCreateInfo,
								    multisampleStateCreateInfo,
								    depthStencilStateCreateInfo,
								    colorBlendStateCreateInfo);
   
    return device.createPipeline(pipelineCreateInfo);
  }

}
