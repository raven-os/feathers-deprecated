#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"
#include "modeset/ModeSetter.hpp"
#include "Exception.hpp"
#include "protocol/WaylandServerProtocol.hpp"

void test(void *user_data, enum wl_protocol_logger_type direction, const struct wl_protocol_logger_message *message)
{
  (void)direction;
  (void)message;
  printf("msg: %s\n", user_data);
}

int main(int argc, char **argv)
{
  protocol::WaylandServerProtocol serverProtocol;

  serverProtocol.AddSocket("Raven");
  serverProtocol.AddProtocolLogger(test, (void *)"salam");

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
