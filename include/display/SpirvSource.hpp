#pragma once

#define SPIRV_SOURCE(NAME)						\
  extern const char NAME##_start;					\
  extern const char NAME##_end;						\
  extern const int NAME##_size;						\
  std::pair<uint32 *, uint32 *> get##NAME##Source()			\
  {									\
    assert(!(NAME##_size % 4));						\
    return {reinterpret_cast<uint *>(&NAME##_start), reinterpret_cast<uint *>(&NAME##_end)} \
  }									\
