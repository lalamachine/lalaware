#pragma once

#include <atomic>
#include <cstdbool>
#include <driver/pulse_cnt.h>
#include <soc/gpio_num.h>

class RotaryKnob
{
public:
  enum class Event { none, left, right };
  RotaryKnob(gpio_num_t a_pin, gpio_num_t b_pin);
  void initialize();
  Event update();

private:
  gpio_num_t a_pin;
  gpio_num_t b_pin;
  std::atomic<int> value;
  pcnt_unit_handle_t pcnt_unit;
  pcnt_channel_handle_t pcnt_chan;
  static bool callback(pcnt_unit_handle_t unit,
                       const pcnt_watch_event_data_t *edata, void *user_ctx);
};
