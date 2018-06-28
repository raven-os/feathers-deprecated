#pragma once

#include <iostream>
#include <wayland-client.h>

namespace display
{
  class UserInput
  {

  protected:
    struct wl_display *wlDisplay{nullptr};

  public:
    UserInput(bool isSubCompositor)
    {
      if (isSubCompositor)
        wlDisplay = wl_display_connect(nullptr);
      else
        std::cout << "HERE initialize input for kernel" << std::endl;
    };
    
    ~UserInput() = default;

    void dispatch()
    {
      if (wlDisplay != nullptr)
        wl_display_dispatch_pending(wlDisplay);
    };
  };
}
