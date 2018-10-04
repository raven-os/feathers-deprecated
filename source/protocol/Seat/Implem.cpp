#include "protocol/Seat/Implem.hpp"

namespace protocol
{
  Implem::Implem(wl_resource *resource) :
    version(wl_resource_get_version(resource))
  {
    seat_client = static_cast<FthSeatClient *>(wl_resource_get_user_data(resource));
  }

  void Implem::release([[maybe_unused]]wl_client *client, wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }

  FthSeatClient *Implem::getSeatClient()
  {
    return seat_client;
  }

  uint32_t Implem::getVersion()
  {
    return version;
  }
}
