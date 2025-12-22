#include "rotary_knob.h"

RotaryKnob::RotaryKnob(gpio_num_t a_pin, gpio_num_t b_pin) :
  a_pin(a_pin), b_pin(b_pin), value(0)
{
}

void RotaryKnob::initialize()
{
  pcnt_unit_config_t unit_config = {.low_limit = -2, .high_limit = 2};
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));
  pcnt_glitch_filter_config_t filter_config = {.max_glitch_ns = 10000};
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));
  pcnt_chan_config_t chan_config = {.edge_gpio_num = a_pin,
                                    .level_gpio_num = b_pin};
  ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
  ESP_ERROR_CHECK(
      pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_HOLD,
                                   PCNT_CHANNEL_EDGE_ACTION_INCREASE));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(
      pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_INVERSE,
      PCNT_CHANNEL_LEVEL_ACTION_KEEP));
  ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, -1));
  ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, 1));
  pcnt_event_callbacks_t callbacks = {.on_reach = callback};
  ESP_ERROR_CHECK(
      pcnt_unit_register_event_callbacks(pcnt_unit, &callbacks, this));
  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

RotaryKnob::Event RotaryKnob::update()
{
  int count = std::atomic_exchange(&value, 0);
  if (count < 0) {
    return Event::left;
  }
  if (count > 0) {
    return Event::right;
  }
  return Event::none;
}

bool RotaryKnob::callback(pcnt_unit_handle_t unit,
                          const pcnt_watch_event_data_t *edata, void *user_ctx)
{
  reinterpret_cast<RotaryKnob *>(user_ctx)->value += edata->watch_point_value;
  pcnt_unit_clear_count(unit);
  return false;
}
