#pragma once

#include "magma/VulkanHandler.hpp"
#include "magma/Device.hpp"
#include "magma/Swapchain.hpp"

namespace display
{
  template<class SurfaceProvider>
  class Display
  {
    magma::Instance instance;
    SurfaceProvider surfaceProvider;
    magma::Device<> device;
    magma::Swapchain<> swapchain;

  public:
    Display();
    Display(Display const &) = delete;
    Display(Display &&) = delete;
    Display operator=(Display const &) = delete;
    Display operator=(Display &&) = delete;
  };
}
