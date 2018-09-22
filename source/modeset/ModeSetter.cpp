#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"

namespace modeset {
  ModeSetter::Drm::Drm()
  {
    // TODO find card1 with proper scan
    fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);
    if (fd < 0)
      {
	throw ModeSettingError("/dev/dri/card1: no such file or directory");
      }

    drmModeRes *res = drmModeGetResources(fd);
    if (!res)
      {
	throw ModeSettingError("Cannot get drm resource");
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

  int ModeSetter::getScreenBufferFd() const
  {
    return drm.fd;
  }
}
