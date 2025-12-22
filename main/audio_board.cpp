#include "audio_board.h"

#include <audio_hal.h>
#include <board_pins_config.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <soc/gpio_num.h>

static const char *TAG = "audio_board";

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

bool AudioBoard::get_headphone_detect()
{
  return gpio_get_level(static_cast<gpio_num_t>(get_headphone_detect_gpio()));
}

void AudioBoard::set_pa_enable(bool pa_enable)
{
  gpio_set_level(static_cast<gpio_num_t>(get_pa_enable_gpio()), pa_enable);
}
