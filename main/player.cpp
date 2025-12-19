#include "player.h"

#include "period.h"

#include <cJSON.h>
#include <cstdbool>
#include <cstdio>
#include <dirent.h>
#include <esp_log.h>
#include <fstream>
#include <queue>
#include <string>

static const char *TAG = "player";

Player::Player() :
  i2c_bus(1, GPIO_NUM_22, GPIO_NUM_5),
  nfc_monitor(i2c_bus),
  push_button(GPIO_NUM_19),
  rotary_knob(GPIO_NUM_23, GPIO_NUM_18)
{
}

void Player::run()
{
  initialize();
  bool playing = false;
  Period period(100);
  while (true) {
    period.wait_next();

    auto status = nfc_monitor.get_status();
    if (status.present) {
      if (!playing) {
        playing = true;
        load_tracks(status.uid);
      }
    } else {
      if (playing) {
        playing = false;
        audio_pipeline.stop();
      }
    }

    audio_board.set_pa_enable(audio_board.get_headphone_detect() && playing);

    auto event = push_button.update();
    if (event == PushButton::Event::short_release) {
      audio_pipeline.skip();
    }
    auto event2 = rotary_knob.update();
    if (event2 == RotaryKnob::Event::minus) {
      int volume = audio_board.get_volume();
      if (volume > 0) {
        audio_board.set_volume(volume - 1);
      }
    }
    if (event2 == RotaryKnob::Event::plus) {
      int volume = audio_board.get_volume();
      if (volume < 100) {
        audio_board.set_volume(volume + 1);
      }
    }
  }
}

void Player::initialize()
{
  audio_board.initialize();
  audio_pipeline.initialize();
  nfc_monitor.start();
  push_button.initialize();
  rotary_knob.initialize();
}

void Player::load_tracks(const std::array<uint8_t, 4> &uid)
{
  char filename[32] = {};
  snprintf(filename, sizeof(filename), "%s/%02X%02X%02X%02X.json", cards_path,
           uid[0], uid[1], uid[2], uid[3]);
  std::ifstream card_file(filename);
  if (card_file.is_open()) {
    std::queue<std::string> tracks;
    std::string data(std::istreambuf_iterator<char>(card_file), {});
    cJSON *json_root = cJSON_ParseWithLength(data.c_str(), data.length());
    if (json_root) {
      cJSON *json_path = cJSON_GetObjectItem(json_root, "path");
      if (json_path && cJSON_IsString(json_path)) {
        std::string path(json_path->valuestring);
        std::string album_path = "/sd/" + path;
        ESP_LOGI(TAG, "listing %s", album_path.c_str());
        DIR *dir = opendir(album_path.c_str());
        if (dir) {
          struct dirent *entry;
          while ((entry = readdir(dir)) != NULL) {
            ESP_LOGI(TAG, "found %s", entry->d_name);
            std::string track_path = album_path + "/" + entry->d_name;
            tracks.push(track_path);
          }
          closedir(dir);
        }
      }
      cJSON *json_files = cJSON_GetObjectItem(json_root, "files");
      if (json_files && cJSON_IsArray(json_files)) {
        cJSON *json_file;
        cJSON_ArrayForEach(json_file, json_files)
        {
          if (cJSON_IsString(json_file)) {
            std::string file(json_file->valuestring);
            std::string tarck_path = "/sd/" + file;
            tracks.push(tarck_path);
          }
        }
      }
      cJSON *json_volume = cJSON_GetObjectItem(json_root, "volume");
      if (json_volume && cJSON_IsNumber(json_volume)) {
        audio_board.set_volume(json_volume->valuedouble);
      }
      cJSON_Delete(json_root);
    }
    audio_pipeline.play(std::move(tracks));
  } else {
    std::ofstream new_file(filename);
    new_file << "{}";
  }
}
