#pragma once

#include <driver/pulse_cnt.h>
#include <soc/gpio_num.h>

class RotaryKnob
{
public:
  enum class Event { none, plus, minus };
  RotaryKnob(gpio_num_t a_pin, gpio_num_t b_pin);
  void initialize();
  Event update();

private:
  gpio_num_t a_pin;
  gpio_num_t b_pin;
  pcnt_unit_handle_t pcnt_unit;
  pcnt_channel_handle_t pcnt_chan;
};
