#include "display/KernelDisplay.hpp"

namespace display
{
  void Compositor::render(WindowTree const &windowTree)
  {
    Frame &frame(frames[frameIndex]);

    renderer.queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::EmptyList{},
								     nullptr,
								     magma::EmptyList{},
								     magma::asListRef(imageAvailable)),
			  nullptr);
    vk::ImageSubresourceRange imageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};


    magma::Semaphore<claws::no_delete> renderDone(renderer.render(device, windowTree, frameIndex, swapchainUserData, frame.data, imageAvailable));

    vk::MappedMemoryRange flushRange{
      frame.imageMemory,
	0,
	VK_WHOLE_SIZE
	};
    vk::PipelineStageFlags waitDestStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

    commandBuffer[0].begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer[0].raw().pipelineBarrier(waitDestStageMask, vk::PipelineStageFlagBits::eHost, {}, {}, {},
					   {
					     vk::ImageMemoryBarrier{
					       vk::AccessFlagBits::eColorAttachmentWrite,
						 vk::AccessFlagBits::eHostRead,
						 vk::ImageLayout::eGeneral,
						 vk::ImageLayout::eGeneral,
						 VK_QUEUE_FAMILY_IGNORED,
						 VK_QUEUE_FAMILY_IGNORED,
						 frame.image,
						 imageSubresourceRange
						 }
					   });

    commandBuffer[0].end();
    renderer.queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::asListRef(renderDone), // wait for rendering to be done
								     &waitDestStageMask,
								     magma::asListRef(commandBuffer[0].raw()),
								     magma::EmptyList{}),
			  fence); // signal the fence
    device.waitForFences({fence}, true, 1000000000);
    device.resetFences({fence});
    {
      auto unmap([&](void const *)
		 {
		   device.flushMappedMemoryRanges({flushRange});
		   device.unmapMemory(frame.imageMemory);
		 });
      std::unique_ptr<void const, decltype(unmap)> data(device.mapMemory(frame.imageMemory, 0, VK_WHOLE_SIZE, {}), unmap);

      device.invalidateMappedMemoryRanges({flushRange});
      quadFullscreen.draw(data.get(), modeSetter.getScreenWidth(), modeSetter.getScreenHeight());
      modeSetter.swapBuffers();
    }
    ++frameIndex %= imageCount;
  }

  modeset::ModeSetter const &Compositor::getModeSetter() const noexcept
  {
    return modeSetter;
  }

  modeset::ModeSetter const &KernelDisplay::getModeSetter() const noexcept
  {
    return renderer.getModeSetter();
  }
}
