#include "display/Display.hpp"
#include "display/WindowTree.hpp"

#include "claws/lambda_utils.hpp"

namespace display
{
  uint32_t Display::Renderer::prepareGpuData(Display::Renderer::FrameData &frame, WindowTree const &windowTree)
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
		       display(child, minDepth + range * 0.5f, range * 0.5f);
		     else
		       delayedRender.push_back(child);
		   range *= 0.5f; // we just gave half our depth to our children
		   {
		     WindowTree::Rect const &rect(windowTree.getData(index).rect);
		     for (uint32_t i(0u); i < 4u; ++i)
		       {
			 for (uint32_t j(0u); j < 2u; ++j)
			   *vertexDataIt++ = float(rect.position[j] + rect.size[j] * ((i >> j) & 1u));
			 *vertexDataIt++ = 0.0f; // minDepth + range;
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
		   float childRange(range / float(delayedRender.size()));
		   float depth(minDepth);
		   for (WindowTree::WindowNodeIndex child : delayedRender)
		     {
		       display(child, depth, depth + childRange);
		       depth += childRange;
		     }
		 });
    claws::inject_self{display}(windowTree.getRootIndex(), 0.0f, 1.0f);
    vertexBuffer.free(frame.vertexBufferRangeId);
    vertexBuffer.free(frame.indexBufferRangeId);
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
    std::cout << "windowCount: " << windowCount << std::endl;
    return windowCount;
  }



  void Display::Renderer::render(WindowTree const &windowTree)
  {
    // get next image data, and image to present
    auto [index, frame] = displaySystem.getImage(imageAvailable);

    // wait for rendering fence
    device.waitForFences({frame.fence}, true, 1000000000);
    // reset fence
    device.resetFences({frame.fence});
    uint32_t const vertexCount(prepareGpuData(frame, windowTree) * 6);
    magma::PrimaryCommandBuffer cmdBuffer(displaySystem.swapchainUserData.commandBuffers[index]);

    // being command recording
    cmdBuffer.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    // we bind our vertex buffer range
    cmdBuffer.bindVertexBuffers(0, {vertexBuffer.getBuffer(frame.vertexBufferRangeId)}, {frame.vertexBufferRangeId.second});
    // we bind our index buffer range
    cmdBuffer.bindIndexBuffer(vertexBuffer.getBuffer(frame.indexBufferRangeId), frame.indexBufferRangeId.second, vk::IndexType::eUint32);
    // we bind update our descriptor so that it points to our image
    cmdBuffer.raw().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, displaySystem.userData.pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
    vk::ClearValue clearValue = {vk::ClearColorValue(std::array<float, 4>{0.0f, 0.5f, 0.0f, 1.0f})}; // a nice recognisable green for debug
    {
      // start the renderpass
      auto lock(cmdBuffer.beginRenderPass(displaySystem.swapchainUserData.renderPass, frame.framebuffer,
					  {{0, 0}, displaySystem.getSwapchain().getExtent()}, {clearValue}, vk::SubpassContents::eInline));

      // us our pipeline
      lock.bindGraphicsPipeline(displaySystem.swapchainUserData.pipeline);
      // draw our quad
      lock.drawIndexed(vertexCount, 1, 0, 0, 0);
    }
    cmdBuffer.end();

    vk::PipelineStageFlags waitDestStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(asListRef(imageAvailable), // wait fo image to be available
							    &waitDestStageMask,
							    magma::asListRef(cmdBuffer.raw()),
							    magma::asListRef(renderDone)), // signal renderdone when done
		 frame.fence); // signal the fence
    //std::cout << "about to present for index " << index << std::endl;
    displaySystem.presentImage(renderDone, index); // present our image
  }



  magma::Pipeline<> Display::Renderer::SwapchainUserData::createPipeline(magma::Device<claws::no_delete> device, magma::Swapchain<claws::no_delete> swapchain, UserData const &userData)
  {
    std::cout << "creating pipeline for swapchain with extent " << swapchain.getExtent().width << ", " << swapchain.getExtent().height << std::endl;
    /// --- Specialisation info --- ///
    // The width and height are specialized rather than being push constants, because a compositor shoudn't change size often (If ever).
    // Both entries have the size of a float
    // The second entry is offset by one float
    std::array<vk::SpecializationMapEntry, 2u> mapEntries{
      vk::SpecializationMapEntry{0, 0, sizeof(float)},
	vk::SpecializationMapEntry{1, sizeof(float), sizeof(float)}
    };
    // These are the actual specialisation values
    std::array<float, 2u> dimensions{static_cast<float>(swapchain.getExtent().width), static_cast<float>(swapchain.getExtent().height)};

    auto specialzationInfo(magma::StructBuilder<vk::SpecializationInfo>::make(mapEntries, sizeof(float) * dimensions.size(), dimensions.data()));
    /// --- Specialisation info END --- ///

    // We have two shaders, and both shader's entrypoints are "main".
    // Only the vertex shader is specialized (see above setup)
    std::vector<vk::PipelineShaderStageCreateInfo>
      shaderStageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, userData.vert, "main", &specialzationInfo},
	{{}, vk::ShaderStageFlagBits::eFragment, userData.frag, "main", nullptr}};

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
    // We don't really care about depth
    vk::Viewport viewport(-static_cast<float>(swapchain.getExtent().width),
			  -static_cast<float>(swapchain.getExtent().height), // pos
			  static_cast<float>(swapchain.getExtent().width) * 2.0f,
			  static_cast<float>(swapchain.getExtent().height) * 2.0f, // size
			  0.0f,
			  1.0); // depth range

    // Scissors cover all that is within the compositor
    vk::Rect2D scissor({0, 0}, swapchain.getExtent());

    auto viewportStateCreateInfo(magma::StructBuilder<vk::PipelineViewportStateCreateInfo, true>::make(magma::asListRef(viewport),
												       magma::asListRef(scissor)));

    // Everything is turned off
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
						     userData.pipelineLayout,
						     renderPass,
						     0);

    pipelineCreateInfo.addRasteringColorAttachementInfo(viewportStateCreateInfo, multisampleStateCreateInfo, colorBlendStateCreateInfo);
    return device.createPipeline(pipelineCreateInfo);
  }

}
