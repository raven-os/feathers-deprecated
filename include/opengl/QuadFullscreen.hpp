#pragma once

# include "my_opengl.hpp"

class QuadFullscreen
{
public:
  QuadFullscreen();
  ~QuadFullscreen();

  void draw(void const * data, GLsizei width, GLsizei height);

private:
  static const int VERTICES_SIZE = 16;

  Texture texture;
  Program program;
  Vao vao;
  glBuffer buffer;
};
