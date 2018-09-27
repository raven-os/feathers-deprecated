#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <libudev.h>
#include <cstring>

#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"

namespace modeset {
  ModeSetter::Drm::Drm()
  {
    fd = scanGpu();
    if (fd < 0)
      {
	throw ModeSettingError("Could not find gpu device");
      }

    drmModeConnector *conn = nullptr;
    for (int i = 0; i < res->count_connectors; ++i)
      {
	conn = drmModeGetConnector(fd, res->connectors[i]);
	if (conn && conn->connection == DRM_MODE_CONNECTED)
	  break;
	drmModeFreeConnector(conn);
	conn = nullptr;
      }
    if (!conn)
      {
	throw ModeSettingError("Connector not found");
      }

    modeInfo = conn->modes[0];
    connectorId = conn->connector_id;

    drmModeEncoder *enc = nullptr;
    if (conn->encoder_id)
      {
	enc = drmModeGetEncoder(fd, conn->encoder_id);
      }
    else
      {
	throw ModeSettingError("Encoder not found");
      }

    if (enc->crtc_id)
      {
	crtc = drmModeGetCrtc(fd, enc->crtc_id);
      }
    else
      {
	throw ModeSettingError("CRTC not found");
      }

    // clean up
    drmModeFreeEncoder(enc);
    drmModeFreeConnector(conn);
    drmModeFreeResources(res);
  }

  int ModeSetter::Drm::scanGpu()
  {
    struct udev *udev = udev_new();
    if (!udev)
      {
	throw ModeSettingError("Could not create udev");
      }

    struct udev_enumerate *en = udev_enumerate_new(udev);
    if (!en)
      {
	throw ModeSettingError("Could not create udev enumerate");
      }

    udev_enumerate_add_match_subsystem(en, "drm");
    udev_enumerate_add_match_sysname(en, "card[0-9]*");
    udev_enumerate_scan_devices(en);

    struct udev_list_entry *entry;

    int gpuFd = -1;

    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(en)) {
      bool isBootVga = false;

      const char *path = udev_list_entry_get_name(entry);
      std::cout << "Testing device " << path << std::endl;
      struct udev_device *dev = udev_device_new_from_syspath(udev, path);
      if (!dev) {
	continue;
      }

      struct udev_device *pci =
	udev_device_get_parent_with_subsystem_devtype(dev, "pci", NULL);

      if (pci) {
	const char *id = udev_device_get_sysattr_value(pci, "boot_vga");
	if (id && strcmp(id, "1") == 0) {
	  isBootVga = true;
	}
      }

      const char *devicePath = udev_device_get_devnode(dev);
      if (!devicePath)
	continue;
      int fd = open(devicePath, O_RDWR|O_CLOEXEC);
      if (fd < 0)
	continue;

      res = drmModeGetResources(fd);
      if (!res) {
	close(fd);
	continue;
      }
      udev_device_unref(dev);
      if (isBootVga)
	{
	  gpuFd = fd;
	  break;
	}
      close(fd);
    }

    udev_unref(udev);
    udev_enumerate_unref(en);
    return gpuFd;
  }

  ModeSetter::Gbm::Gbm(int fd, uint16_t width, uint16_t height)
  {
    gbmDevice = gbm_create_device(fd);
    eglDisplay = eglGetDisplay(gbmDevice);
    eglInitialize(eglDisplay, nullptr, nullptr);

    // create an OpenGL context
    eglBindAPI(EGL_OPENGL_API);
    EGLint attributes[] = {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_NONE};
    EGLConfig config;
    EGLint numConfig;
    eglChooseConfig(eglDisplay, attributes, &config, 1, &numConfig);
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, nullptr);

    // create the GBM and EGL surface
    gbmSurface = gbm_surface_create(gbmDevice,
				    height,
				    width,
				    GBM_BO_FORMAT_XRGB8888,
				    GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    eglSurface = eglCreateWindowSurface(eglDisplay, config, gbmSurface, nullptr);
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
  }

  ModeSetter::ModeSetter()
    : drm(),
      gbm(drm.fd, drm.modeInfo.vdisplay, drm.modeInfo.hdisplay),
      previousBo(nullptr),
      previousFb(0)
  {
  }

  ModeSetter::~ModeSetter()
  {
    // set the previous crtc
    drmModeSetCrtc(drm.fd,
		   drm.crtc->crtc_id,
		   drm.crtc->buffer_id,
		   drm.crtc->x,
		   drm.crtc->y,
		   &drm.connectorId, 1, &drm.crtc->mode);
    drmModeFreeCrtc(drm.crtc);

    if (previousBo)
      {
	drmModeRmFB(drm.fd, previousFb);
	gbm_surface_release_buffer(gbm.gbmSurface, previousBo);
      }

    eglDestroySurface(gbm.eglDisplay, gbm.eglSurface);
    eglDestroyContext(gbm.eglDisplay, gbm.eglContext);
    eglTerminate(gbm.eglDisplay);
    gbm_device_destroy(gbm.gbmDevice);

    close(drm.fd);
  }

  void ModeSetter::swapBuffers()
  {
    eglSwapBuffers(gbm.eglDisplay, gbm.eglSurface);
    struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbm.gbmSurface);
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    uint32_t stride = gbm_bo_get_stride(bo);
    uint32_t fb;
    drmModeAddFB(drm.fd,
		 drm.modeInfo.hdisplay,
		 drm.modeInfo.vdisplay,
		 24, 32, stride, handle, &fb);
    int ret = drmModeSetCrtc(drm.fd,
			     drm.crtc->crtc_id,
			     fb,
			     0,
			     0,
			     &drm.connectorId, 1, &drm.modeInfo);
    if (ret != 0)
      {
	throw ModeSettingError("Cannot set CRTC");
      }

    if (previousBo) {
      drmModeRmFB(drm.fd, previousFb);
      gbm_surface_release_buffer(gbm.gbmSurface, previousBo);
    }

    previousBo = bo;
    previousFb = fb;
  }

  int ModeSetter::getScreenWidth() const
  {
    return drm.modeInfo.hdisplay;
  }

  int ModeSetter::getScreenHeight() const
  {
    return drm.modeInfo.vdisplay;
  }
}
