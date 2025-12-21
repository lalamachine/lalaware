#pragma once

#include <audio_element.h>
#include <audio_pipeline.h>
#include <cstdbool>
#include <cstddef>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class AudioPipeline
{
public:
  AudioPipeline();
  void initialize();
  void play(std::vector<std::string> &&tracks, bool loop);
  void skip();
  void stop();

private:
  audio_pipeline_handle_t audio_pipeline;
  audio_element_handle_t fatfs_stream;
  audio_element_handle_t mp3_decoder;
  audio_element_handle_t rsp_filter;
  audio_element_handle_t i2s_stream;
  audio_event_iface_handle_t audio_event_iface;
  std::vector<std::string> tracks;
  bool loop;
  size_t position;
  std::thread thread;
  std::mutex mutex;
  void entry();
  void play_next();
  void terminate();
};
