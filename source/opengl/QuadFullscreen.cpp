#include "opengl/QuadFullscreen.hpp"
#include "opengl/my_opengl.hpp"

#include <iostream>
#include <cassert>

QuadFullscreen::QuadFullscreen()
  : program(opengl::createProgram("texture"))
{
  constexpr float const vertices[]
    {
    // pos        | tex pos
      -1.0f,  1.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 1.0,
       1.0f, -1.0f, 1.0f, 1.0f,
    };

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void QuadFullscreen::draw(opengl::Texture const &texture, GLsizei width, GLsizei height)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  opengl::checkError();
  glUseProgram(program);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
