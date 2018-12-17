#pragma once

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define EGL_EGLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglmesaext.h>

struct wl_display;

namespace modeset {
  /*
   * Class that handles kernel mode setting
   */
  class ModeSetter
  {
    struct Drm
    {
      Drm();

      int scanGpu();

      int fd;
      uint32_t connectorId;
      drmModeModeInfo modeInfo;
      drmModeCrtc *crtc;
      drmModeRes *res;
    };

    struct Gbm
    {
      Gbm(int fd, uint16_t width, uint16_t height);

      struct gbm_device *gbmDevice;
      struct gbm_surface *gbmSurface;
      EGLDisplay eglDisplay;
      EGLContext eglContext;
      EGLSurface eglSurface;

      void bindWaylandDisplay(struct wl_display *display) const;
    };

  public:
    ModeSetter();
    ModeSetter(ModeSetter const &) = delete;
    ~ModeSetter();

    void swapBuffers();
    int getScreenWidth() const;
    int getScreenHeight() const;
    void bindWaylandDisplay(struct wl_display *display) const;

  private:
    Drm drm;
    Gbm gbm;

    struct gbm_bo *previousBo;
    uint32_t previousFb;
  };
}
