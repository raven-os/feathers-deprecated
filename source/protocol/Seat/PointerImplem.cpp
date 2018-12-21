#include <iostream>
#include "protocol/Surface.hpp"
#include "protocol/Seat/PointerImplem.hpp"

namespace protocol
{
  PointerImplem::PointerImplem(wl_resource *resource)
  {
    version = wl_resource_get_version(resource);
    fthPointer = new FthPointer();
  }

  void PointerImplem::set_cursor([[maybe_unused]]wl_client *client,
				 [[maybe_unused]]wl_resource *resource,
				 [[maybe_unused]]uint32_t serial,
				 [[maybe_unused]]wl_resource *surface_resource,
				 [[maybe_unused]]int32_t hotspot_x,
				 [[maybe_unused]]int32_t hotspot_y)
  {
    FthPointer *pointer = static_cast<FthPointer*>(wl_resource_get_user_data(resource));
    Surface *surface;

    // if (client != pointer->focus.client)
    //   return;

    // if (pointer->cursor.surface) {
    //   surface_set_view(pointer->cursor.surface, NULL);
    //   wl_list_remove(&pointer->cursor.destroy_listener.link);
    // }

    surface = surface_resource ? static_cast<Surface*>(wl_resource_get_user_data(surface_resource)) : nullptr;
    // pointer->cursor.surface = surface;
    // pointer->cursor.hotspot.x = hotspot_x;
    // pointer->cursor.hotspot.y = hotspot_y;

    // if (surface) {
    //   surface_set_view(surface, &pointer->cursor.view);
    //   wl_resource_add_destroy_listener(surface->resource, &pointer->cursor.destroy_listener);
    //   update_cursor(pointer);
    // }
    std::cout << "Pointer set" << std::endl;
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
    // if (!(seat_client->seat->capabilities & WL_SEAT_CAPABILITY_POINTER))
    // {
    //   std::cerr << "Client sent get_pointer on seat without the pointer capability" << std::endl;
    //   return ;
    // }
    instantiateImplementation(client, version, id, wl_pointer_interface, &pointer_implementation, fthPointer, [](wl_resource *){
       std::cout << "Pointer unbind!" << std::endl;
    });

    std::cout << "Pointer binded" << std::endl;
  }
}
