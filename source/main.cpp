#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "evdev-client/EvdevClient.hpp"
#include "modeset/ModeSetter.hpp"
#include "opengl/QuadFullscreen.hpp"
#include "Exception.hpp"
#include "display/WindowTree.hpp"

void addTestWindows(display::WindowTree &windowTree)
{
  {
    auto root(windowTree.getRootIndex());
    auto child(windowTree.addChild(root));

    auto &childData(windowTree.getData(child));

    childData.rect.position[0] = 10;
    childData.rect.position[1] = 40;
    childData.rect.size[0] = 100;
    childData.rect.size[1] = 100;
    childData.isSolid = true;
    for (int i = 0; i < 4; ++i)
      {
	auto grandChild(windowTree.addChild(child));
	auto &grandChildData(windowTree.getData(grandChild));

	grandChildData.rect.position[0] = 50 + i * 10;
	grandChildData.rect.position[1] = 50 + i * 60;
	grandChildData.rect.size[0] = 200;
	grandChildData.rect.size[1] = 50;
	grandChildData.isSolid = true;
      }
  }
}

int main(int argc, char **argv)
{
  display::WindowTree windowTree(display::WindowData
				 {{{{0, 0}}, {{600, 400}}}, true });

  addTestWindows(windowTree);
  if (argc == 1)
    {
      // RUN ON TTY
      try
	{
	  ModeSetter modeSetter;
	  EvdevClient evdevC;

	  evdevC.initClient();
	  for (int i = 0; i < 600; ++i)
	    {
	      quadFullscreen.draw();
	      modeSetter.swapBuffers();
	      evdevC.loop();
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
	  display.render(windowTree);
	  waylandSurface.dispatch();
	  //  std::cout << "presenting image" << std::endl;
	}
    }

  std::cout << "Exit" << std::endl;
  return 0;
}
