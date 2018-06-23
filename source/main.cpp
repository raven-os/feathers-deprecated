#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"
#include "protocol/WaylandServerProtocol.hpp"

static void debug_server_protocol(void *user_data,
				  enum wl_protocol_logger_type,
				  const struct wl_protocol_logger_message *)
{
  printf("msg: %s\n", static_cast<char *>(user_data));
}

int main(int argc, char **argv)
{
  protocol::WaylandServerProtocol serverProtocol;

  for (int32_t i = 1; i < argc - 1; ++i)
    {
      if (!strncmp(argv[i], "--socket", 8))
	{
	  int socket_nb = atoi(argv[i] + 8);
	  switch (socket_nb)
	    {
	    case 1 ... 2:
	      printf("AddSocket method with name '%s' was %s\n", argv[i + 1], serverProtocol.AddSocket(argv[i + 1]) ? "unsuccessful" : "successful");
	      serverProtocol.AddProtocolLogger(debug_server_protocol,
					       static_cast<void *>(argv[i] + 8));
	    default:
	      break;
	    }
	}
    }

  if (argc == 1)
    {
      // RUN ON TTY
      try
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
