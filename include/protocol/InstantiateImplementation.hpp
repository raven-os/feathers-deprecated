#pragma once

namespace protocol
{
  inline wl_resource *instantiateImplementation(wl_client *client, uint32_t version, uint32_t id, wl_interface const &interface, void *implementation,
						void *data = nullptr, void (*destroy)(wl_resource *) = [](wl_resource *){})
  {
    if (wl_resource *resource = wl_resource_create(client, &interface, version, id))
      {
	wl_resource_set_implementation(resource, implementation, data, destroy);
	return resource;
      }
    else
      {
	wl_client_post_no_memory(client);
	return nullptr;
      }
  }
}
