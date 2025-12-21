#pragma once

#include <cstdint>
#include <esp_random.h>
#include <limits>

class RandomGenerator : public std::numeric_limits<uint32_t>
{
public:
  typedef uint32_t result_type;
  uint32_t operator()()
  {
    return esp_random();
  }
};
