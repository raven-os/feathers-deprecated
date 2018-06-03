#pragma once

extern "C"
{
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>
};

/*
 * Class that handles kernel mode setting
 */
class ModeSetter
{
public:
  ModeSetter();
  ~ModeSetter();

  void swapBuffers();

private:
  void initDRM();
  void initGBM();

  // DRM
  int fd;
  uint32_t connectorId;
  drmModeModeInfo modeInfo;
  drmModeCrtc *crtc;

  // GBM
  struct gbm_device *gbmDevice;
  EGLDisplay eglDisplay;
  EGLContext eglContext;
  struct gbm_surface *gbmSurface;
  EGLSurface eglSurface;

  struct gbm_bo *previousBo;
  uint32_t previousFb;
};
