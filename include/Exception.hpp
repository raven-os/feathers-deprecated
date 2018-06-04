#pragma once

#include <exception>
#include <string>

class Exception : public std::exception {
public:
  Exception(std::string const& msg) : msg(msg) {}
  virtual ~Exception() noexcept {}

  char const *what() const noexcept { return msg.c_str(); }

private:
  std::string msg;
};

class ModeSettingError : public Exception {
public:
  ModeSettingError(std::string const& msg) : Exception(msg) {}
};
