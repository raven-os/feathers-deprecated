#pragma once

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>

/*
 * Class that handles kernel mode setting
 */
class ModeSetter
{
  struct Drm
  {
    Drm();

    int fd;
    uint32_t connectorId;
    drmModeModeInfo modeInfo;
    drmModeCrtc *crtc;
  };

  struct Gbm
  {
    Gbm(int fd, uint16_t hDisplay, uint16_t vDisplay);

    struct gbm_device *gbmDevice;
    struct gbm_surface *gbmSurface;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
  };

public:
  ModeSetter();
  ~ModeSetter();

  void swapBuffers();

private:
  Drm drm;
  Gbm gbm;

  struct gbm_bo *previousBo;
  uint32_t previousFb;
};
