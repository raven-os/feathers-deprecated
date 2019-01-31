#include "protocol/Buffer.hpp"

#include "display/Renderer.hpp"

#include <cassert>

namespace protocol
{
  void Buffer::destroy(wl_client *client, wl_resource *resource)
  {
    std::visit([client, resource](auto &data)
	       {
		 data.destroy(client, resource);
	       }, data);
  }

  void ShmBuffer::update(display::Renderer *renderer)
  {
    magma::DynamicBuffer::RangeId tmpBuffer(renderer->getStagingBuffer().allocate((width * height * 4 + 0x40) & ~0x3F));
    {
      auto memory(renderer->getStagingBuffer().getMemory<unsigned char []>(tmpBuffer));
      auto it(&memory[0]);
      
      for (int y(0u); y < height; ++y)
	{
	  auto ptr(data->data + offset + y * stride);
	  it = std::copy(ptr, ptr + width * 4, it);
	}
    }
    magma::Device<claws::no_delete> device(renderer->getDevice());
    vk::PhysicalDevice physicalDevice(renderer->getPhysicalDevice());

    if (!image)
      {
	image = device.createImage2D({}, vk::Format::eR8G8B8A8Unorm, {width, height}, vk::SampleCountFlagBits::e1,
				     vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageLayout::eUndefined);

	auto memRequirements(device.getImageMemoryRequirements(image));

	imageMemory = device.selectAndCreateDeviceMemory(physicalDevice, memRequirements.size, vk::MemoryPropertyFlagBits::eDeviceLocal, memRequirements.memoryTypeBits);
	device.bindImageMemory(image, imageMemory, 0);

	imageView = device.createImageView({},
					   image,
					   vk::ImageViewType::e2D,
					   vk::Format::eR8G8B8A8Unorm,
					   vk::ComponentMapping{},
					   vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
								     0,
								     1,
								     0,
								     1));
      }
    renderer->uploadBuffer(tmpBuffer, image, width, height);
  }

  void ShmBuffer::destroy(wl_client *, wl_resource *)
  {
    assert(data);
    data = nullptr;
  }
}
