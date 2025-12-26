#include "player.h"

#include "period.h"
#include "random_generator.h"

#include <algorithm>
#include <cJSON.h>
#include <cstdio>
#include <dirent.h>
#include <esp_log.h>
#include <fstream>

static const char *TAG = "player";

Player::Player() :
  i2c_bus(1, GPIO_NUM_22, GPIO_NUM_5),
  nfc_monitor(i2c_bus),
  push_button(GPIO_NUM_19),
  rotary_knob(GPIO_NUM_23, GPIO_NUM_18),
  playing(false),
  volume(0)
{
}

void Player::run()
{
  initialize();
  volume = audio_board.get_volume();

  Period period(100);
  while (true) {
    period.wait_next();

    auto status = nfc_monitor.get_status();
    if (status.present && audio_board.sdcard_is_mounted()) {
      if (!playing) {
        playing = true;
        ESP_LOGI(TAG, "card inserted, UID=%02X%02X%02X%02X", status.uid[0],
                 status.uid[1], status.uid[2], status.uid[3]);
        load_tracks(status.uid);
      }
    } else {
      if (playing) {
        playing = false;
        ESP_LOGI(TAG, "card removed");
        audio_pipeline.stop();
      }
    }

    audio_board.process(playing);

    auto event = push_button.update();
    if (event == PushButton::Event::short_release) {
      audio_pipeline.skip();
    }
    auto event2 = rotary_knob.update();
    if (event2 == RotaryKnob::Event::left) {
      if (volume > 1) {
        volume--;
        audio_board.set_volume(volume);
      }
    }
    if (event2 == RotaryKnob::Event::right) {
      if (volume < 100) {
        volume++;
        audio_board.set_volume(volume);
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
    std::vector<std::string> tracks;
    bool loop = false;
    std::string data(std::istreambuf_iterator<char>(card_file), {});
    cJSON *json_root = cJSON_ParseWithLength(data.c_str(), data.length());
    if (json_root) {
      cJSON *json_path = cJSON_GetObjectItem(json_root, "path");
      if (json_path && cJSON_IsString(json_path)) {
        std::string album_path = "/sd/" + std::string(json_path->valuestring);
        list_files(album_path, tracks);
      }
      cJSON *json_files = cJSON_GetObjectItem(json_root, "files");
      if (json_files && cJSON_IsArray(json_files)) {
        cJSON *json_file;
        cJSON_ArrayForEach(json_file, json_files)
        {
          if (cJSON_IsString(json_file)) {
            std::string track_path =
                "/sd/" + std::string(json_file->valuestring);
            tracks.push_back(track_path);
          }
        }
      }
      cJSON *json_loop = cJSON_GetObjectItem(json_root, "loop");
      if (json_loop && cJSON_IsTrue(json_loop)) {
        loop = true;
      }
      cJSON *json_shuffle = cJSON_GetObjectItem(json_root, "shuffle");
      if (json_shuffle && cJSON_IsTrue(json_shuffle)) {
        std::shuffle(tracks.begin(), tracks.end(), RandomGenerator());
      }
      cJSON *json_volume = cJSON_GetObjectItem(json_root, "volume");
      if (json_volume && cJSON_IsNumber(json_volume)) {
        volume = std::ranges::clamp(static_cast<int>(json_volume->valuedouble),
                                    1, 100);
        audio_board.set_volume(volume);
      }
      cJSON_Delete(json_root);
    }
    audio_pipeline.play(std::move(tracks), loop);
  } else {
    std::ofstream new_file(filename);
    new_file << "{}";
  }
}

void Player::list_files(const std::string &path,
                        std::vector<std::string> &files)
{
  ESP_LOGI(TAG, "listing %s", path.c_str());
  DIR *dir = opendir(path.c_str());
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      ESP_LOGI(TAG, "found %s", entry->d_name);
      std::string file = path + "/" + entry->d_name;
      files.push_back(file);
    }
    closedir(dir);
  }
  std::sort(files.begin(), files.end());
}
