#include <iostream>
#include "protocol/Seat/FeatherPointer.hpp"

namespace protocol {

  FthPointer::FthPointer() : Input()
  {
    std::cout << "Pointer bind !" << std::endl;
  }

  void FthPointer::enter(struct wl_list *resources)
  {

  }

  void FthPointer::leave(struct wl_list *resources)
  {

  }

}
