#pragma once

#include <EGL/egl.h>
#include <vulkan/vulkan.h>

namespace display
{
  EGLImage createEglImageFromVkMemory(VkDevice device,
				      VkDeviceMemory memory,
				      EGLDisplay display,
				      EGLint width,
				      EGLint height);
}

