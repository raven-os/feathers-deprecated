#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "protocol/Seat/LibInput.hpp"

namespace protocol
{
  LibInput::LibInput()
  {
    libinputInterface = libinput_interface {
      [](const char* path, int flags, void *data)
      {
          return static_cast<LibInput *>(data)->handleOpenRestricted(path, flags);
      },
      [](int fd, void *data)
      {
          return static_cast<LibInput *>(data)->handleCloseRestricted(fd);
      },
    };

    #ifdef ENABLE_LIBUDEV
    	if (!(udev = udev_new())) {
    		std::cerr << "Could not create udev context\n" << std::endl;
    		return;
    	}

    	libInput = libinput_udev_create_context(&libinputInterface, NULL, udev);
      if (!libInput) {
    		std::cerr << "Could not create libInput context\n" << std::endl;
        udev_unref(udev);
    		return;
    	}
      if (libinput_udev_assign_seat(libInput, "seat0") != 0) {
    		std::cerr << "Failed to assign seat to libInput context\n" << std::endl;
        libinput_unref(libInput);
    		udev_unref(udev);
        return;
      }

    //#else

    /*	libInput = libinput_netlink_create_context(&libinputInterface, NULL);
      if (!libInput) {
        std::cerr << "Could not create libInput context\n" << std::endl;
        return;
      }
      if (libinput_netlink_assign_seat(libInput, "seat0") != 0) {
    		std::cerr << "Failed to assign seat to libInput context\n" << std::endl;
    		libinput_unref(libInput);
        return;
    	}*/
    #endif

  	// libinputSource = wl_event_loop_add_fd(swc.event_loop, libinput_get_fd(libInput), WL_EVENT_READABLE,
  	// 	 &handle_libinput_data, NULL);

  	if (!libinputSource) {
  		std::cerr << "Could not create event source for libInput\n" << std::endl;
  		libinput_unref(libInput);
      #ifdef ENABLE_LIBUDEV
        	udev_unref(udev);
      #endif
      return;
  	}

  	// if (!swc.active)
  	// 	libinput_suspend(libInput);
  }

  LibInput::~LibInput()
  {

  }

  int LibInput::handleOpenRestricted(const char *path, int flags)
  {
    //TODO seems we have to return the 'in' socket fd
    return -1;
  }

  void LibInput::handleCloseRestricted(int fd)
  {
    close(fd);
  }

  libinput const& LibInput::getLibInput() const
  {
    return *libInput;
  }
}
