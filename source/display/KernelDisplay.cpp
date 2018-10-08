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
    commandBuffer[0].raw().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {},
					   {
					     vk::BufferMemoryBarrier{
					       vk::AccessFlagBits::eMemoryRead,
						 vk::AccessFlagBits::eTransferWrite,
						 VK_QUEUE_FAMILY_IGNORED,
						 VK_QUEUE_FAMILY_IGNORED,
						 frame.buffer,
						 0,
						 VK_WHOLE_SIZE
						 }
					   }, {});
    commandBuffer[0].raw().pipelineBarrier(waitDestStageMask, vk::PipelineStageFlagBits::eTransfer, {}, {}, {},
					   {
					     vk::ImageMemoryBarrier{
					       vk::AccessFlagBits::eColorAttachmentWrite,
						 vk::AccessFlagBits::eTransferRead,
						 vk::ImageLayout::eGeneral,
						 vk::ImageLayout::eTransferSrcOptimal,
						 VK_QUEUE_FAMILY_IGNORED,
						 VK_QUEUE_FAMILY_IGNORED,
						 frame.image,
						 imageSubresourceRange
						 }
					   });
    // commandBuffer[0].raw().copyImageToBuffer(frame.image,
    // 					     vk::ImageLayout::eTransferSrcOptimal,
    // 					     frame.buffer,
    // 					     {
    // 					       vk::BufferImageCopy(0,
    // 								   modeSetter.getScreenWidth(),
    // 								   modeSetter.getScreenHeight(),
    // 								   vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
    // 											      0,
    // 											      0,
    // 											      1),
    // 								   vk::Offset3D(0, 0, 0),
    // 								   vk::Extent3D(modeSetter.getScreenWidth(),
    // 										modeSetter.getScreenHeight(),
    // 										1))
    // 						 });
    commandBuffer[0].raw().fillBuffer(frame.buffer,
				      0,
				      VK_WHOLE_SIZE,
				      0xFFFFFFFF);

    commandBuffer[0].raw().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, waitDestStageMask, {}, {}, {},
					   {
					     vk::ImageMemoryBarrier{
					       vk::AccessFlagBits::eTransferRead,
						 vk::AccessFlagBits::eColorAttachmentWrite,
						 vk::ImageLayout::eTransferSrcOptimal,
						 vk::ImageLayout::eGeneral,
						 VK_QUEUE_FAMILY_IGNORED,
						 VK_QUEUE_FAMILY_IGNORED,
						 frame.image,
						 imageSubresourceRange
						 }
					   });
    commandBuffer[0].raw().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe, {}, {},
					   {
					     vk::BufferMemoryBarrier{
					       vk::AccessFlagBits::eTransferWrite,
						 vk::AccessFlagBits::eMemoryRead,
						 VK_QUEUE_FAMILY_IGNORED,
						 VK_QUEUE_FAMILY_IGNORED,
						 frame.buffer,
						 0,
						 VK_WHOLE_SIZE
						 }
					   }, {});
    commandBuffer[0].end();
    renderer.queue.submit(magma::StructBuilder<vk::SubmitInfo>::make(magma::asListRef(renderDone), // wait for rendering to be done
								     &waitDestStageMask,
								     magma::asListRef(commandBuffer[0].raw()),
								     magma::EmptyList{}),
			  fence); // signal the fence
    device.waitForFences({fence}, true, 1000000000);
    device.resetFences({fence});
    quadFullscreen.draw(frame.texture, modeSetter.getScreenWidth(), modeSetter.getScreenHeight());
    modeSetter.swapBuffers();
    ++frameIndex %= imageCount;
  }
}
