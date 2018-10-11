#include "protocol/Seat/PointerImplem.hpp"

namespace protocol
{
  PointerImplem::PointerImplem(wl_resource *resource) : Implem(resource)
  {
  }

  void PointerImplem::set_cursor([[maybe_unused]]wl_client *client,
        [[maybe_unused]]wl_resource *resource,
        [[maybe_unused]]uint32_t serial,
        [[maybe_unused]]wl_resource *surface,
        [[maybe_unused]]int32_t hotspot_x,
        [[maybe_unused]]int32_t hotspot_y)
  {

  }

  void PointerImplem::release([[maybe_unused]]wl_client *client, wl_resource *resource)
  {
    wl_resource_destroy(resource);
  }

  void PointerImplem::createImplem(wl_client *client, uint32_t id)
  {
    static auto pointer_implementation(createImplementation<struct wl_pointer_interface,
                &PointerImplem::set_cursor,
                &PointerImplem::release
                >());

    if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_POINTER))
    {
      std::cerr << "Client sent get_pointer on seat without the pointer capability" << std::endl;
      return ;
    }
    instantiateImplementation(client, version, id, wl_pointer_interface, &pointer_implementation, seat_client, [](wl_resource *){
       std::cout << "Pointer created!" << std::endl;
    });
  }
}
