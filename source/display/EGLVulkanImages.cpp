#include "display/EGLVulkanImages.hpp"

#include <EGL/eglext.h>
#include <drm_fourcc.h>

#include <stdexcept>

namespace display
{
  namespace impl
  {
    namespace
    {
      VkResult vkGetMemoryFdKHR(VkDevice device,
				VkMemoryGetFdInfoKHR const *info,
				int *fd)
      {
	static auto func = reinterpret_cast<PFN_vkGetMemoryFdKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryFdKHR"));
	if (func)
	  return func(device, info, fd);
	throw std::runtime_error("vulkan: Failed to load vkGetMemoryFdKHR");
      }
    }
  }


  EGLImage createEglImageFromVkMemory(VkDevice device,
				      VkDeviceMemory memory,
				      EGLDisplay display,
				      EGLint width,
				      EGLint height)
  {
    int fd;
    {
      VkMemoryGetFdInfoKHR const info
      {
	VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
	  nullptr,
	  memory,
	  VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT
	  };
      impl::vkGetMemoryFdKHR(device, &info, &fd);
    }
      
    EGLAttrib const attr[] = {
      EGL_WIDTH, width,
      EGL_HEIGHT, height,
      EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_RGBA8888,
      EGL_DMA_BUF_PLANE0_FD_EXT, fd,
      EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
      EGL_DMA_BUF_PLANE0_PITCH_EXT, width * 4,
      EGL_NONE
    };
    return eglCreateImage(display,
			  EGL_NO_CONTEXT,
			  EGL_LINUX_DMA_BUF_EXT,
			  nullptr,
			  attr);
  }
}
