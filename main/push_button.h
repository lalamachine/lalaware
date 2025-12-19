#pragma once

#include <cstdint>
#include <soc/gpio_num.h>

class PushButton
{
public:
  enum class Event { none, push, short_release, long_release };
  PushButton(gpio_num_t pin);
  void initialize();
  Event update();

private:
  gpio_num_t pin;
  uint8_t state;
};
