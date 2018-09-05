#include "display/WaylandSurface.hpp"
#include "display/Display.hpp"
#include "display/KernelDisplay.hpp"
#include "Exception.hpp"
#include "display/WindowTree.hpp"
#include "protocol/WaylandServerProtocol.hpp"

static void debug_server_protocol(void *user_data,
				  enum wl_protocol_logger_type,
				  const struct wl_protocol_logger_message *)
{
  printf("msg: %s\n", static_cast<char *>(user_data));
}

void addTestWindows(display::WindowTree &windowTree)
{
  {
    auto root(windowTree.getRootIndex());
    auto child(windowTree.addChild(root));

    auto &childData(windowTree.getData(child));

    childData.rect.position[0] = 10;
    childData.rect.position[1] = 40;
    childData.rect.size[0] = 300;
    childData.rect.size[1] = 300;
    childData.isSolid = true;
    for (int i = 0; i < 4; ++i)
      {
	auto grandChild(windowTree.addChild(child));
	auto &grandChildData(windowTree.getData(grandChild));

	grandChildData.rect.position[0] = 100 + i * 20;
	grandChildData.rect.position[1] = 100 + i * 120;
	grandChildData.rect.size[0] = 400;
	grandChildData.rect.size[1] = 100;
	grandChildData.isSolid = true;
      }
  }
}

int main(int argc, char **argv)
{
  display::WindowTree windowTree(display::WindowData
				 {{{{0, 0}}, {{1920, 1080}}}, true});

  addTestWindows(windowTree);
  if (argc == 1)
    {
      // RUN ON TTY
      try
	{
	  display::KernelDisplay kernelDisplay;

	  for (int i = 0; i < 120; ++i)
	    {
	      kernelDisplay.render(windowTree);
	    }
	}
      catch (ModeSettingError const& e)
	{
	  std::cerr << e.what() << std::endl;
	}
      catch (std::runtime_error const &e)
	{
	  std::cerr << e.what() << std::endl;
	}
    }
  else if (!strcmp(argv[1], "-sc") || !strcmp(argv[1], "--sub-compositor"))
    {
      protocol::WaylandServerProtocol serverProtocol;
      std::string socketname("");

      for (int32_t i = 1; i < argc - 1; ++i)
	{
	  if (!strcmp(argv[i], "--socket"))
	    {
	      printf("Attempting to connect to server with socket '%s'\n",
		     argv[i + 1]);
	      socketname = argv[i + 1];
	    }
	  else if (!strcmp(argv[i], "--client-socket"))
	    {
	      printf("AddSocket method with name '%s' was %s\n",
		     argv[i + 1], serverProtocol.AddSocket(argv[i + 1]) ?
		     "unsuccessful" : "successful");
	      serverProtocol.EventDispatch(0);
	      serverProtocol.AddProtocolLogger(static_cast<wl_protocol_logger_func_t>
					       (debug_server_protocol),
					       static_cast<void *>(argv[i] + 8));
	      serverProtocol.EventDispatch(0);
	    }
	}
      display::WaylandSurface waylandSurface(socketname);
      display::Display display(waylandSurface);

      while (waylandSurface.isRunning())
	{
	  display.render(windowTree);
	  waylandSurface.dispatch();
	}
    }

  std::cout << "Exit" << std::endl;
  return 0;
}
