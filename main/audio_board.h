#pragma once

#include <board.h>
#include <cstdbool>

class AudioBoard
{
public:
  void initialize();
  int get_volume();
  void set_volume(int volume);
  bool sdcard_is_mounted();
  bool get_headphone_detect();
  void set_pa_enable(bool pa_enable);

private:
  audio_board_handle_t board_handle;
};
