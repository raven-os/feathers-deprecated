#include "display/WaylandSurface.hpp"
#include "display/Display.hpp"
#include "display/KernelDisplay.hpp"
#include "Exception.hpp"
#include "display/WindowTree.hpp"
#include "protocol/WaylandServerProtocol.hpp"
#include "Args.hpp"

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


static void help(char const *name)
{
  printf("Usage: %s [OPTIONS]...\n"
	 "\n"
	 "\t-H, --help\t\t dislay this help and exit\n"
	 "\t-T, --tty\t\t tty mode\n"
	 "\t-E, --sub-compositor\t sub-compositor mode\n"
	 "\t-c, --client-socket\t clients sockets names (only in sub-compositor mode)\n"
	 "\t-s, --socket\t\t weston socket name (default if not specify)\n\n"
	 , name);
}


int main(int argc, char **argv)
{
  display::WindowTree windowTree(display::WindowData
				 {{{{0, 0}}, {{1920, 1080}}}, true});
  protocol::WaylandServerProtocol serverProtocol;
  struct Args args;

  while (1)
    {
      static const struct option long_options[] =
        {
         {"help", no_argument, 0, 'h'},
         {"tty", no_argument, 0, 'T'},
         {"sub-compositor", no_argument, 0, 'E'},
         {"client-socket", required_argument, 0, 'c'},
         {"socket", required_argument, 0, 's'},
         {0, 0, 0, 0}
	};
      int option_index = 0;
      int c = getopt_long (argc, argv, "hTEc:s:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
	case 'T':
          if (args.mode == 1) {
            puts("Mode cannot be TTY and SubCompositor\n");
            return -1;
          }
          args.mode = 0;
          break;

	case 'E':
          if (args.mode == 0) {
            puts("Mode cannot be SubCompositor and TTY\n");
            return -1;
          }
          args.mode = 1;
          break;

	case 'c':
          if (args.mode != 1) {
            puts("Cannot set client-socket in non SubCompositor modes\n");
            return -1;
          }
          args.socketName = optarg;
          break;

	case 's':
          args.clientSocketsNames.push_back(optarg);
          break;

	case 'h':
	  help(argv[0]);
	  return 0;

	case '?':
          break;

	default:
	  puts("Unkown error.\n");
          return -1;
	}
    }

  if (optind < argc)
    {
      printf("non-option ARGV-elements: ");
      while (optind < argc)
	printf("%s ", argv[optind++]);
      putchar('\n');
    }

  if (args.mode == -1)
    {
      puts("Mode's not set");
      return -1;
    }

  for (auto it = args.clientSocketsNames.begin(); it < args.clientSocketsNames.end(); it++)
    {
      printf("AddSocket method with name '%s' was %s\n",
	     it->c_str(), serverProtocol.addSocket(*it) ?
	     "unsuccessful" : "successful");
      serverProtocol.eventDispatch(0);
      // serverProtocol.addProtocolLogger(static_cast<wl_protocol_logger_func_t>
      // 				       (debug_server_protocol),
      // 				       static_cast<void *>(it->c_str()));
      // serverProtocol.eventDispatch(0);
    }

  addTestWindows(windowTree);

  if (args.mode)
    {
      std::string socketname("");
      printf("Attempting to connect to server with socket '%s'\n", args.socketName.c_str());
      display::WaylandSurface waylandSurface(args.socketName);
      display::Display display(waylandSurface);

      while (waylandSurface.isRunning())
	{
	  display.render(windowTree);
	  waylandSurface.dispatch();
	  serverProtocol.eventDispatch(0);
	}
    }
  else
    {
      // RUN ON TTY
      try
	{
	  display::KernelDisplay kernelDisplay;

	  for (int i = 0; i < 120; ++i)
	    {
	      kernelDisplay.render(windowTree);
	      serverProtocol.eventDispatch(0);
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

  std::cout << "Exit" << std::endl;
  return 0;
}
