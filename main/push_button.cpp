#include "push_button.h"

#include <cstdint>
#include <driver/gpio.h>
#include <hal/gpio_types.h>
#include <limits>

PushButton::PushButton(gpio_num_t pin) : pin(pin), state(0) {}

void PushButton::initialize()
{
  gpio_set_direction(pin, GPIO_MODE_INPUT);
  gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

PushButton::Event PushButton::update()
{
  Event event = Event::none;
  if (!gpio_get_level(pin)) {
    if (state == 0) {
      event = Event::push;
    }
    if (state < std::numeric_limits<uint8_t>::max()) {
      state++;
    }
  } else {
    if (state != 0) {
      if (state < 5) {
        event = Event::short_release;
      } else {
        event = Event::long_release;
      }
    }
    state = 0;
  }
  return event;
}
