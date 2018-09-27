#include "display/WaylandSurface.hpp"
#include "display/Display.hpp"
#include "display/KernelDisplay.hpp"
#include "Exception.hpp"
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

int main(int argc, char **argv)
{
  protocol::WaylandServerProtocol serverProtocol;

  for (int32_t i = 1; i < argc - 1; ++i)
    if (!strcmp(argv[i], "--client-socket"))
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


  if (argc > 1 && (!strcmp(argv[1], "-sc") || !strcmp(argv[1], "--sub-compositor")))
    {
      std::string socketname("");

      for (int32_t i = 1; i < argc - 1; ++i)
	{
	  if (!strcmp(argv[i], "--socket"))
	    {
	      printf("Attempting to connect to server with socket '%s'\n",
		     argv[i + 1]);
	      socketname = argv[i + 1];
	    }
	}
      display::WaylandSurface waylandSurface(socketname);
      display::Display display(waylandSurface);

      while (waylandSurface.isRunning())
	{
	  display.render(serverProtocol.getWindowTree());
	  waylandSurface.dispatch();
	  serverProtocol.eventDispatch(0);
	}
    }
  else
    {
      {
	// RUN ON TTY
	try
	  {
	    display::KernelDisplay kernelDisplay;

	    for (int i = 0; i < 120; ++i)
	      {
		kernelDisplay.render(serverProtocol.getWindowTree());
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

    }

  std::cout << "Exit" << std::endl;
  return 0;
}
