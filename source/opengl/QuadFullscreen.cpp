#include "opengl/QuadFullscreen.hpp"
#include "opengl/my_opengl.hpp"

QuadFullscreen::QuadFullscreen()
  : texture(my_opengl::loadTexture("resource/BackgroundSpace.bmp")),
    program(my_opengl::createProgram("texture")),
    vertices{
      // pos        // tex pos
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f, 1.0,
      1.0f,  1.0f,  1.0f, 1.0f,
    }
{
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
}

QuadFullscreen::~QuadFullscreen()
{
}

void QuadFullscreen::draw()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glBindVertexArray(vao);
  glUseProgram(program);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE * sizeof(float), vertices, GL_STATIC_DRAW);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
