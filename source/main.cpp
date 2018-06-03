#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "modeset/ModeSetter.hpp"

#include <unistd.h>

int main(int argc, char **argv)
{
  if (argc == 1)
    {
      display::WaylandSurface waylandSurface;
      display::Display display(waylandSurface);

      while (true)
	{
	  display.render();
	  waylandSurface.dispatch();
	  std::cout << "presenting image" << std::endl;
	}
    }
  else
    {
      ModeSetter modeSetter;
      for (int i = 0; i < 600; ++i)
	{
	  float progress = i / 600.0f;
	  glClearColor (1.0f - progress, progress, 0.0, 1.0);
	  glClear (GL_COLOR_BUFFER_BIT);
	  modeSetter.swapBuffers();
	  usleep(100);
	}
    }
}
