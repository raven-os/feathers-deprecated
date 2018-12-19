#pragma once

#include <libinput.h>
#include <linux/input.h>
#ifdef ENABLE_LIBUDEV
  #include <libudev.h>
#endif
#include <wayland-server.h>

namespace protocol
{
  class LibInput
  {
  public:
    LibInput();
    ~LibInput();

    libinput const& getLibInput() const;

  private:
    libinput_interface libinputInterface;

    libinput *libInput;
    wl_event_source *libinputSource;
    #ifdef ENABLE_LIBUDEV
	   udev *udev;
    #endif

  private:
    int handleOpenRestricted(const char *path, int flags, void *userData);
    void handleCloseRestricted(int fd, void *userData);
  };
}
