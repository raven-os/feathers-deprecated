#include "protocol/Seat/KeyboardImplem.hpp"

namespace protocol
{
  void KeyboardImplem::release(wl_client *client, wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }
}
