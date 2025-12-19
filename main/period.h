#pragma once

#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Period
{
public:
  Period(uint32_t period)
  {
    last_wake = xTaskGetTickCount();
    increment = pdMS_TO_TICKS(period);
  }

  void wait_next()
  {
    xTaskDelayUntil(&last_wake, increment);
  }

private:
  TickType_t last_wake;
  TickType_t increment;
};
