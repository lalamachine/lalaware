#include "board.h"

#include <audio_mem.h>
#include <esp_err.h>
#include <esp_peripherals.h>
#include <periph_sdcard.h>

static audio_board_handle_t board_handle = 0;

extern audio_hal_func_t AUDIO_CODEC_ES8388_DEFAULT_HANDLE;

audio_board_handle_t audio_board_init()
{
  board_handle = audio_calloc(1, sizeof(struct audio_board_handle));
  board_handle->audio_hal = audio_board_codec_init();

  esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
  esp_periph_set_handle_t periph_set = esp_periph_set_init(&periph_cfg);
  ESP_ERROR_CHECK(audio_board_sdcard_init(periph_set));

  return board_handle;
}

audio_hal_handle_t audio_board_codec_init()
{
  audio_hal_codec_config_t audio_codec_cfg = {
      .adc_input = AUDIO_HAL_ADC_INPUT_LINE1,
      .dac_output = AUDIO_HAL_DAC_OUTPUT_ALL,
      .codec_mode = AUDIO_HAL_CODEC_MODE_BOTH,
      .i2s_iface = {.mode = AUDIO_HAL_MODE_SLAVE,
                    .fmt = AUDIO_HAL_I2S_NORMAL,
                    .samples = AUDIO_HAL_48K_SAMPLES,
                    .bits = AUDIO_HAL_BIT_LENGTH_16BITS}};
  return audio_hal_init(&audio_codec_cfg, &AUDIO_CODEC_ES8388_DEFAULT_HANDLE);
}

esp_err_t audio_board_sdcard_init(esp_periph_set_handle_t set)
{
  periph_sdcard_cfg_t sdcard_cfg = {
      .card_detect_pin = get_sdcard_intr_gpio(),
      .root = "/sd",
      .mode = SD_MODE_4_LINE,
  };
  esp_periph_handle_t sdcard_handle = periph_sdcard_init(&sdcard_cfg);
  return esp_periph_start(set, sdcard_handle);
}

audio_board_handle_t audio_board_get_handle()
{
  return board_handle;
}
