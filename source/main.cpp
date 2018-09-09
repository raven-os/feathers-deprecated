#include "display/WaylandSurface.hpp"
#include "display/Display.hpp"
#include "display/KernelDisplay.hpp"
#include "Exception.hpp"
#include "display/WindowTree.hpp"
#include "protocol/WaylandServerProtocol.hpp"

static void debug_server_protocol([[maybe_unused]]void *user_data,
				  enum wl_protocol_logger_type type,
				  const struct wl_protocol_logger_message *message)
{
  printf("msg: [%s]\n", type == WL_PROTOCOL_LOGGER_REQUEST ? "REQUEST" : "EVENT");
  printf("message->name: %s\n", message->message->name);
  printf("message->signature: %s\n", message->message->signature);
  printf("message data:\n{\n");
  for (unsigned int i(0u); message->message->signature[i]; ++i)
    {
      char c(message->message->signature[i]);
      wl_argument arg(message->arguments[i]);
      switch (c)
	{
	case 'i':
	  printf("\t%i\n", arg.i);
	  break;
	case 'u':
	  printf("\t%u\n", arg.u);
	  break;
	case 'f':
	  printf("\t%i\n", arg.f);
	  break;
	case 's':
	  printf("\t\"%s\"\n", arg.s);
	  break;
	default:
	  printf("\t[unhandled]\n");
	  break;
	}
    }
  printf("}\n");
}

static void addTestWindows(display::WindowTree &windowTree)
{
  {
    auto root(windowTree.getRootIndex());
    auto child(windowTree.addChild(root));

    auto &childData(windowTree.getData(child));

    childData.rect.position[0] = 10u;
    childData.rect.position[1] = 40u;
    childData.rect.size[0] = 300u;
    childData.rect.size[1] = 300u;
    childData.isSolid = true;
    for (uint32_t i = 0; i < 4; ++i)
      {
	auto grandChild(windowTree.addChild(child));
	auto &grandChildData(windowTree.getData(grandChild));

	grandChildData.rect.position[0] = uint16_t(100u + i * 20u);
	grandChildData.rect.position[1] = uint16_t(100u + i * 120u);
	grandChildData.rect.size[0] = 400u;
	grandChildData.rect.size[1] = 100u;
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

      for (int32_t i = 2; i < argc - 1; ++i)
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
		     argv[i + 1], serverProtocol.addSocket(argv[i + 1]) ?
		     "unsuccessful" : "successful");
	      serverProtocol.eventDispatch(0);
	      serverProtocol.addProtocolLogger(static_cast<wl_protocol_logger_func_t>
					       (debug_server_protocol),
					       static_cast<void *>(argv[i + 1]));
	      serverProtocol.eventDispatch(0);
	    }
	}
      display::WaylandSurface waylandSurface(socketname);
      display::Display display(waylandSurface);

      while (waylandSurface.isRunning())
	{
	  display.render(windowTree);
	  waylandSurface.dispatch();
	  serverProtocol.eventDispatch(0);
	}
    }

  std::cout << "Exit" << std::endl;
  return 0;
}
