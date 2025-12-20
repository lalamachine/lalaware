#pragma once

#include <audio_hal.h>
#include <board_pins_config.h>
#include <esp_peripherals.h>

#define ESP_SD_PIN_CLK GPIO_NUM_14
#define ESP_SD_PIN_CMD GPIO_NUM_15
#define ESP_SD_PIN_D0 GPIO_NUM_2
#define ESP_SD_PIN_D1 GPIO_NUM_4
#define ESP_SD_PIN_D2 GPIO_NUM_12
#define ESP_SD_PIN_D3 GPIO_NUM_13

#define BOARD_PA_GAIN 4

#ifdef __cplusplus
extern "C" {
#endif

struct audio_board_handle
{
  audio_hal_handle_t audio_hal;
};

typedef struct audio_board_handle *audio_board_handle_t;

audio_board_handle_t audio_board_init();

audio_hal_handle_t audio_board_codec_init();

esp_err_t audio_board_sdcard_init(esp_periph_set_handle_t set);

audio_board_handle_t audio_board_get_handle();

#ifdef __cplusplus
}
#endif
