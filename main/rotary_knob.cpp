#include "rotary_knob.h"

RotaryKnob::RotaryKnob(gpio_num_t a_pin, gpio_num_t b_pin) :
  a_pin(a_pin), b_pin(b_pin)
{
}

void RotaryKnob::initialize()
{
  pcnt_unit_config_t unit_config = {.low_limit = -10, .high_limit = 10};
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));
  pcnt_glitch_filter_config_t filter_config = {.max_glitch_ns = 10000};
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));
  pcnt_chan_config_t chan_config = {.edge_gpio_num = a_pin,
                                    .level_gpio_num = b_pin};
  ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
  ESP_ERROR_CHECK(
      pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                   PCNT_CHANNEL_EDGE_ACTION_HOLD));
  ESP_ERROR_CHECK(
      pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                    PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

RotaryKnob::Event RotaryKnob::update()
{
  int count;
  pcnt_unit_get_count(pcnt_unit, &count);
  if (count > 0) {
    pcnt_unit_clear_count(pcnt_unit);
    return Event::plus;
  }
  if (count < 0) {
    pcnt_unit_clear_count(pcnt_unit);
    return Event::minus;
  }
  return Event::none;
}
