#pragma once

#include <wayland-server.h>
#include <utility>

namespace protocol
{
  namespace impl
  {
    template<class MemberType>
    struct member_ptr_func_traits;

    template<class ThisType, class ReturnType, class... Params>
    struct member_ptr_func_traits<ReturnType (ThisType::*)(Params...)>
    {
      using this_type = ThisType;

      template<template<class...> class tp>
      using copy_params = tp<Params...>;
    };

    template<class MemberType>
    using this_type_t = typename member_ptr_func_traits<MemberType>::this_type;

    template<class... Params>
    struct callMemberStructImpl
    {
      template<class MemberType, MemberType member, class... AdditionalParams>
      static constexpr auto callMemberImpl(struct wl_client *client,
					   struct wl_resource *resource,
					   AdditionalParams &&... additionalParms)
      {
	using this_type = this_type_t<MemberType>;
	(static_cast<this_type *>(wl_resource_get_user_data(resource))->*member)(client, resource, std::forward<AdditionalParams>(additionalParms)...);
      }

      template<auto member>
      static constexpr auto callMember(Params... params)
      {
	return callMemberImpl<decltype(member), member>(params...);
      }
    };


    template<auto member>
    constexpr auto getFuncPtr()
    {
      using callMemberStruct = typename member_ptr_func_traits<decltype(member)>::template copy_params<callMemberStructImpl>;
      return &callMemberStruct::template callMember<member>;
    }
  }

  template<class Implemenation, auto ... members>
  constexpr Implemenation createImplementation()
  {
    return Implemenation{impl::getFuncPtr<members>()...};
  }
}
