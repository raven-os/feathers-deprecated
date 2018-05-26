#include "display/WaylandSurface.hpp"
#include "display/Display.ipp"

#include <unistd.h>

int main()
{
  display::Display<display::WaylandSurface> display;
  magma::Semaphore<> sem;

  while (true)
    {
      display.render();
      std::cout << "presenting image" << std::endl;
    }
}
