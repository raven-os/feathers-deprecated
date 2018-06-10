#include "opengl/my_opengl.hpp"
#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"

int main(int argc, char **argv)
{
  if (argc == 1)
    {
      // RUN ON TTY
      try
	{
	  ModeSetter modeSetter;
	  // TODO
	  Texture texture = my_opengl::loadTexture("../../resource/BackgroundSpace.bmp");

	  for (int i = 0; i < 600; ++i)
	    {
	      float progress = i / 600.0f;

	      glClearColor(1.0f - progress, progress, 0.0, 1.0);
	      glClear(GL_COLOR_BUFFER_BIT);
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
