#include "audio_board.h"

#include <audio_hal.h>
#include <driver/gpio.h>
#include <es8388.h>

void AudioBoard::initialize()
{
  board_handle = audio_board_init();
  audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE,
                       AUDIO_HAL_CTRL_START);
  es8388_write_reg(ES8388_DACCONTROL26, 0x1E);
  es8388_write_reg(ES8388_DACCONTROL27, 0x1E);
  gpio_set_direction(HEADPHONE_DETECT, GPIO_MODE_INPUT);
  gpio_set_direction(PA_ENABLE_GPIO, GPIO_MODE_OUTPUT);
}

int AudioBoard::get_volume()
{
  int volume;
  board_handle->audio_hal->audio_codec_get_volume(&volume);
  return volume;
}

void AudioBoard::set_volume(int volume)
{
  board_handle->audio_hal->audio_codec_set_volume(volume);
}

bool AudioBoard::get_headphone_detect()
{
  return gpio_get_level(HEADPHONE_DETECT);
}

void AudioBoard::set_pa_enable(bool pa_enable)
{
  gpio_set_level(PA_ENABLE_GPIO, pa_enable);
}
