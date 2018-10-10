#pragma once

# include "my_opengl.hpp"
# include "EGL/egl.h"

class QuadFullscreen
{
public:
  QuadFullscreen();
  ~QuadFullscreen() = default;

  void draw(opengl::Texture const &texture, GLsizei width, GLsizei height);

private:
  static const int VERTICES_SIZE = 16;

  opengl::Program program;
  opengl::Buffer buffer;
};
