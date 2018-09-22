#pragma once

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>

namespace modeset {
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
      Gbm(int fd, uint16_t width, uint16_t height);

      struct gbm_device *gbmDevice;
      struct gbm_surface *gbmSurface;
      EGLDisplay eglDisplay;
      EGLContext eglContext;
      EGLSurface eglSurface;
    };

  public:
    ModeSetter();
    ModeSetter(ModeSetter const &) = delete;
    ~ModeSetter();

    void swapBuffers();
    int getScreenBufferFd() const;
    int getScreenWidth() const;
    int getScreenHeight() const;

  private:
    Drm drm;
    Gbm gbm;

    struct gbm_bo *previousBo;
    uint32_t previousFb;
  };
}
