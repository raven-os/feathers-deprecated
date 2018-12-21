#pragma once

#include "protocol/Input.hpp"

namespace protocol
{

  class FthPointer : public Input
  {
  public:
    FthPointer();
    FthPointer(FthPointer const &) = delete;
    FthPointer(FthPointer &&) = delete;

    void enter(struct wl_list *resources) override;
    void leave(struct wl_list *resources) override;

  private:
    struct wl_array buttons;
    struct wl_list handlers;
    wl_fixed_t x;
    wl_fixed_t y;
  };
}
