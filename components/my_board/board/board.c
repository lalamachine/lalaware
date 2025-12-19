#include "board.h"

#include "audio_mem.h"
#include "esp_log.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"

static const char *TAG = "AUDIO_BOARD";

static audio_board_handle_t board_handle = 0;

extern audio_hal_func_t AUDIO_CODEC_ES8388_DEFAULT_HANDLE;

audio_board_handle_t audio_board_init(void)
{
  if (board_handle) {
    ESP_LOGW(TAG, "The board has already been initialized!");
    return board_handle;
  }
  board_handle =
      (audio_board_handle_t) audio_calloc(1, sizeof(struct audio_board_handle));
  AUDIO_MEM_CHECK(TAG, board_handle, return NULL);
  board_handle->audio_hal = audio_board_codec_init();

  esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
  esp_periph_set_handle_t periph_set = esp_periph_set_init(&periph_cfg);
  audio_board_sdcard_init(periph_set);

  return board_handle;
}

audio_hal_handle_t audio_board_codec_init(void)
{
  audio_hal_codec_config_t audio_codec_cfg = {
      .adc_input = AUDIO_HAL_ADC_INPUT_LINE1,
      .dac_output = AUDIO_HAL_DAC_OUTPUT_ALL,
      .codec_mode = AUDIO_HAL_CODEC_MODE_BOTH,
      .i2s_iface = {.mode = AUDIO_HAL_MODE_SLAVE,
                    .fmt = AUDIO_HAL_I2S_NORMAL,
                    .samples = AUDIO_HAL_48K_SAMPLES,
                    .bits = AUDIO_HAL_BIT_LENGTH_16BITS}};
  audio_hal_handle_t codec_hal =
      audio_hal_init(&audio_codec_cfg, &AUDIO_CODEC_ES8388_DEFAULT_HANDLE);
  AUDIO_NULL_CHECK(TAG, codec_hal, return NULL);
  return codec_hal;
}

esp_err_t audio_board_sdcard_init(esp_periph_set_handle_t set)
{
  periph_sdcard_cfg_t sdcard_cfg = {
      .card_detect_pin = get_sdcard_intr_gpio(),
      .root = "/sd",
      .mode = SD_MODE_1_LINE,
  };
  esp_periph_handle_t sdcard_handle = periph_sdcard_init(&sdcard_cfg);
  esp_err_t ret = esp_periph_start(set, sdcard_handle);
  int retry_time = 5;
  bool mount_flag = false;
  while (retry_time--) {
    if (periph_sdcard_is_mounted(sdcard_handle)) {
      mount_flag = true;
      break;
    } else {
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
  if (mount_flag == false) {
    ESP_LOGE(TAG, "Sdcard mount failed");
    return ESP_FAIL;
  }
  return ret;
}

audio_board_handle_t audio_board_get_handle(void)
{
  return board_handle;
}

esp_err_t audio_board_deinit(audio_board_handle_t audio_board)
{
  esp_err_t ret = ESP_OK;
  ret = audio_hal_deinit(audio_board->audio_hal);
  audio_free(audio_board);
  board_handle = NULL;
  return ret;
}
