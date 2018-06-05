#pragma once

#include <stdexcept>
#include <string>

class ModeSettingError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
