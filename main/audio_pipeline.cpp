#include "audio_pipeline.h"

#include "audio_element.h"

#include <audio_pipeline.h>
#include <cstdbool>
#include <esp_err.h>
#include <esp_log.h>
#include <fatfs_stream.h>
#include <filter_resample.h>
#include <i2s_stream.h>
#include <mp3_decoder.h>
#include <string>
#include <utility>

static const char *TAG = "audio_pipeline";

void AudioPipeline::initialize()
{
  audio_pipeline_cfg_t audio_pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
  audio_pipeline = audio_pipeline_init(&audio_pipeline_cfg);
  mem_assert(pipeline);

  fatfs_stream_cfg_t fatfs_stream_cfg = FATFS_STREAM_CFG_DEFAULT();
  fatfs_stream_cfg.type = AUDIO_STREAM_READER;
  fatfs_stream = fatfs_stream_init(&fatfs_stream_cfg);

  mp3_decoder_cfg_t mp3_decoder_cfg = DEFAULT_MP3_DECODER_CONFIG();
  mp3_decoder = mp3_decoder_init(&mp3_decoder_cfg);

  rsp_filter_cfg_t rsp_filter_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
  rsp_filter = rsp_filter_init(&rsp_filter_cfg);

  i2s_stream_cfg_t i2s_stream_cfg = I2S_STREAM_CFG_DEFAULT();
  i2s_stream_cfg.type = AUDIO_STREAM_WRITER;
  i2s_stream = i2s_stream_init(&i2s_stream_cfg);
  i2s_stream_set_clk(i2s_stream, 48000, 16, 2);

  audio_pipeline_register(audio_pipeline, fatfs_stream, "file");
  audio_pipeline_register(audio_pipeline, mp3_decoder, "mp3");
  audio_pipeline_register(audio_pipeline, rsp_filter, "filter");
  audio_pipeline_register(audio_pipeline, i2s_stream, "i2s");

  const char *link_tag[4] = {"file", "mp3", "filter", "i2s"};
  audio_pipeline_link(audio_pipeline, &link_tag[0], 4);

  audio_event_iface_cfg_t audio_event_iface_cfg =
      AUDIO_EVENT_IFACE_DEFAULT_CFG();
  audio_event_iface = audio_event_iface_init(&audio_event_iface_cfg);

  audio_pipeline_set_listener(audio_pipeline, audio_event_iface);

  thread = std::thread(&AudioPipeline::entry, this);
}

void AudioPipeline::play(std::queue<std::string> &&tracks)
{
  std::lock_guard<std::mutex> guard(mutex);
  std::swap(queue, tracks);
  play_next();
}

void AudioPipeline::skip()
{
  std::lock_guard<std::mutex> guard(mutex);
  terminate();
  play_next();
}

void AudioPipeline::stop()
{
  std::lock_guard<std::mutex> guard(mutex);
  terminate();
}

void AudioPipeline::entry()
{
  while (true) {
    audio_event_iface_msg_t msg;
    if (audio_event_iface_listen(audio_event_iface, &msg, portMAX_DELAY) ==
        ESP_OK) {
      std::lock_guard<std::mutex> guard(mutex);
      if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT) {
        if (msg.source == mp3_decoder &&
            msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
          audio_element_info_t info = {};
          audio_element_getinfo(mp3_decoder, &info);
          audio_element_setinfo(i2s_stream, &info);
          rsp_filter_set_src_info(rsp_filter, info.sample_rates, info.channels);
        }
        if (msg.source == i2s_stream && msg.cmd == AEL_MSG_CMD_REPORT_STATUS) {
          if (audio_element_get_state(i2s_stream) == AEL_STATE_FINISHED) {
            play_next();
          }
        }
        if (msg.source == fatfs_stream &&
            msg.cmd == AEL_MSG_CMD_REPORT_STATUS) {
          if (audio_element_get_state(fatfs_stream) == AEL_STATE_ERROR) {
            ESP_LOGE(TAG, "failed to open file");
            terminate();
            play_next();
          }
        }
        if (msg.source == mp3_decoder && msg.cmd == AEL_MSG_CMD_REPORT_STATUS) {
          if (audio_element_get_state(mp3_decoder) == AEL_STATE_ERROR) {
            ESP_LOGE(TAG, "failed to decode file");
            terminate();
            play_next();
          }
        }
      }
    }
  }
}

void AudioPipeline::play_next()
{
  if (!queue.empty()) {
    ESP_LOGI(TAG, "playing %s", queue.front().c_str());
    audio_element_set_uri(fatfs_stream, queue.front().c_str());
    audio_pipeline_reset_ringbuffer(audio_pipeline);
    audio_pipeline_reset_elements(audio_pipeline);
    audio_pipeline_change_state(audio_pipeline, AEL_STATE_INIT);
    audio_pipeline_run(audio_pipeline);
    queue.pop();
  } else {
    ESP_LOGI(TAG, "nothing to play");
    terminate();
  }
}

void AudioPipeline::terminate()
{
  audio_pipeline_stop(audio_pipeline);
  audio_pipeline_wait_for_stop(audio_pipeline);
  audio_pipeline_terminate(audio_pipeline);
}
