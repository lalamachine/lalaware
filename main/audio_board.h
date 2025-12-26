#pragma once

#include <board.h>
#include <cstdbool>

class AudioBoard
{
public:
  AudioBoard();
  void initialize();
  void process(bool playing);
  int get_volume();
  void set_volume(int volume);
  bool sdcard_is_mounted();

private:
  audio_board_handle_t board_handle;
  bool mono;
};
