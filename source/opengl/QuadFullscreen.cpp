#include "opengl/QuadFullscreen.hpp"
#include "opengl/my_opengl.hpp"

#include <iostream>

QuadFullscreen::QuadFullscreen()
  : texture(),
    program(my_opengl::createProgram("texture"))
{
  constexpr float const vertices[]
    {
    // pos        | tex pos
      -1.0f,  1.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 1.0,
       1.0f, -1.0f, 1.0f, 1.0f,
    };

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

QuadFullscreen::~QuadFullscreen()
{
}

void QuadFullscreen::draw(void const *buffer, GLsizei width, GLsizei height)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,
	       0,
	       GL_SRGB8_ALPHA8,
	       width,
	       height,
	       0,
	       GL_RGBA,
	       GL_UNSIGNED_BYTE,
	       buffer);
  if (int err = glGetError())
    {
      switch (err)
	{
	case GL_INVALID_ENUM:
	  std::cerr << "GL_INVALID_ENUM\n";
	  break;
	case GL_INVALID_VALUE:
	  std::cerr << "GL_INVALID_VALUE\n";
	  break;
	case GL_INVALID_OPERATION:
	  std::cerr << "GL_INVALID_OPERATION\n";
	  break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
	  std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
	  break;
	case GL_OUT_OF_MEMORY:
	  std::cerr << "GL_OUT_OF_MEMORY\n";
	  break;
	default:
	  std::cerr << "Unknown GL error\n";
	}
    }
  glBindVertexArray(vao);
  glUseProgram(program);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
