#include <iostream>
#include <sstream>
#include <fstream>
#include "opengl/my_opengl.hpp"
#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"

Program initProgram()
{
  std::string name("texture");
  std::stringstream vert;
  std::stringstream frag;
  std::ifstream vertInput("shaders/" + name + ".vert");
  std::ifstream fragInput("shaders/" + name + ".frag");

  if (!fragInput || !vertInput)
    {
      std::cout << "shaders/" + name + ".vert" << std::endl;
      std::cout << "shaders/" + name + ".frag" << std::endl;
      throw std::runtime_error(strerror(errno));
    }

  vert << vertInput.rdbuf();
  frag << fragInput.rdbuf();

  Shader vertex = my_opengl::createShader(GL_VERTEX_SHADER, vert.str().c_str());
  Shader fragment = my_opengl::createShader(GL_FRAGMENT_SHADER, frag.str().c_str());

  return my_opengl::createProgram<2>({vertex, fragment});
}

int main(int argc, char **argv)
{
  if (argc == 1)
    {
      // RUN ON TTY
      try
	{
	  ModeSetter modeSetter;

	  Texture texture = my_opengl::loadTexture("resource/BackgroundSpace.bmp");

	  float vertices[] =
	    {
	      // pos        // tex pos
	      -1.0f, -1.0f, 0.0f, 0.0f,
	      1.0f, -1.0f,  1.0f, 0.0f,
	      -1.0f,  1.0f, 0.0f, 1.0,
	      1.0f,  1.0f,  1.0f, 1.0f,
	    };

	  Vao vao;
	  glBindVertexArray(vao);
	  glBuffer textureBuffer;
	  glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	  glEnableVertexAttribArray(0);
	  glEnableVertexAttribArray(1);
	  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	  Program program = initProgram();

	  for (int i = 0; i < 60; ++i)
	    {
	      float progress = i / 600.0f;

	      glClearColor(1.0f - progress, progress, 0.0, 1.0);
	      glClear(GL_COLOR_BUFFER_BIT);

	      glActiveTexture(GL_TEXTURE0);
	      glBindTexture(GL_TEXTURE_2D, texture);

	      glBindVertexArray(vao);
	      glUseProgram(program);
	      glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	      glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);
	      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	      modeSetter.swapBuffers();
	      usleep(100);
	    }
	}
      catch (ModeSettingError const& e)
	{
	  std::cerr << e.what() << std::endl;
	}
    }
  else if (!strcmp(argv[1], "-sc") || !strcmp(argv[1], "--sub-compositor"))
    {
      display::WaylandSurface waylandSurface;
      display::Display display(waylandSurface);

      while (waylandSurface.isRunning())
	{
	  display.render();
	  waylandSurface.dispatch();
	  //  std::cout << "presenting image" << std::endl;
	}
    }

  std::cout << "Exit" << std::endl;
  return 0;
}
