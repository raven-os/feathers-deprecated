#include "protocol/Seat/PointerImplem.hpp"

namespace protocol
{
  PointerImplem::PointerImplem(wl_resource *resource) : Implem(resource)
  {
  }

  void PointerImplem::set_cursor(wl_client *client,
        wl_resource *resource,
        uint32_t serial,
        wl_resource *surface,
        int32_t hotspot_x,
        int32_t hotspot_y)
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
    if (wl_resource *resource = wl_resource_create(client, &wl_pointer_interface, version, id))
    {
     wl_resource_set_implementation(resource, &pointer_implementation, seat_client, [](wl_resource *){
       std::cout << "Pointer created!" << std::endl;
      });
    }
    else
      wl_client_post_no_memory(client);
  }
}
