#include "protocol/Seat/FeatherKeyboard.hpp"

namespace protocol {

  FthKeyboard::FthKeyboard() : Input()
  {

  }

  bool FthKeyboard::key(uint32_t time, uint32_t state)
  {

  }

  bool FthKeyboard::modifiers()
  {

  }

  void FthKeyboard::enter(struct wl_list *resources)
  {
  
  }

  void FthKeyboard::leave(struct wl_list *resources)
  {

  }

  uint32_t const FthKeyboard::getRate() const
  {
    return repeatInfo.rate;
  }

  uint32_t const FthKeyboard::getDelay() const
  {
    return repeatInfo.delay;
  }

  XkbHandler& FthKeyboard::getXkbHandler()
  {
    return xkb;
  }

}
