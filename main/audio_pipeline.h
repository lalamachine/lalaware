#pragma once

#include <audio_element.h>
#include <audio_pipeline.h>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class AudioPipeline
{
public:
  void initialize();
  void play(std::queue<std::string> &&tracks);
  void skip();
  void stop();

private:
  audio_pipeline_handle_t audio_pipeline;
  audio_element_handle_t fatfs_stream;
  audio_element_handle_t mp3_decoder;
  audio_element_handle_t rsp_filter;
  audio_element_handle_t i2s_stream;
  audio_event_iface_handle_t audio_event_iface;
  std::queue<std::string> queue;
  std::thread thread;
  std::mutex mutex;
  void entry();
  void play_next();
  void terminate();
};
