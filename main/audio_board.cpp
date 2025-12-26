#include "audio_board.h"

#include <audio_hal.h>
#include <board_pins_config.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <hal/gpio_types.h>
#include <periph_sdcard.h>
#include <soc/gpio_num.h>

static const char *TAG = "audio_board";

AudioBoard::AudioBoard() : mono(false) {}

void AudioBoard::initialize()
{
  board_handle = audio_board_init();
  audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE,
                       AUDIO_HAL_CTRL_START);
  gpio_set_direction(static_cast<gpio_num_t>(get_headphone_detect_gpio()),
                     GPIO_MODE_INPUT);
  gpio_set_direction(static_cast<gpio_num_t>(get_pa_enable_gpio()),
                     GPIO_MODE_OUTPUT);
}

void AudioBoard::process(bool playing)
{
  bool headphone_detect =
      gpio_get_level(static_cast<gpio_num_t>(get_headphone_detect_gpio()));
  gpio_set_level(static_cast<gpio_num_t>(get_pa_enable_gpio()),
                 headphone_detect && playing);
  if (headphone_detect) {
    if (!mono) {
      ESP_LOGI(TAG, "switching to mono");
      mono = true;
      board_handle->audio_hal->audio_codec_enable_pa(mono);
    }
  } else {
    if (mono) {
      ESP_LOGI(TAG, "switching to stereo");
      mono = false;
      board_handle->audio_hal->audio_codec_enable_pa(mono);
    }
  }
}

int AudioBoard::get_volume()
{
  int volume;
  board_handle->audio_hal->audio_codec_get_volume(&volume);
  return volume;
}

void AudioBoard::set_volume(int volume)
{
  ESP_LOGI(TAG, "setting volume %d", volume);
  board_handle->audio_hal->audio_codec_set_volume(volume);
}

bool AudioBoard::sdcard_is_mounted()
{
  return periph_sdcard_is_mounted(board_handle->sdcard);
}
