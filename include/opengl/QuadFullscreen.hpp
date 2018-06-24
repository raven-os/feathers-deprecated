#ifndef QUADFULLSCREEN_HPP_
# define QUADFULLSCREEN_HPP_

# include "my_opengl.hpp"

class QuadFullscreen
{
public:
  QuadFullscreen();
  ~QuadFullscreen();

  void draw();

private:
  static const int VERTICES_SIZE = 16;

  Texture texture;
  Program program;
  float vertices[VERTICES_SIZE];
  Vao vao;
  glBuffer buffer;
};

#endif /* !QUADFULLSCREEN_HPP_ */
