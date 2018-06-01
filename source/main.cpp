#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"

#include <unistd.h>

int main()
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
