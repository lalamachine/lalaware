#pragma once

#include "audio_board.h"
#include "audio_pipeline.h"
#include "i2c_bus.h"
#include "nfc_monitor.h"
#include "push_button.h"
#include "rotary_knob.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class Player
{
public:
  Player();
  void run();

private:
  static constexpr char cards_path[] = "/sd/cards";
  AudioBoard audio_board;
  AudioPipeline audio_pipeline;
  I2cBus i2c_bus;
  NfcMonitor nfc_monitor;
  PushButton push_button;
  RotaryKnob rotary_knob;
  void initialize();
  void load_tracks(const std::array<uint8_t, 4> &uid);
  void list_files(const std::string &path, std::vector<std::string> &files);
};
