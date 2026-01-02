#include "settings.h"

#include <algorithm>
#include <cJSON.h>
#include <fstream>
#include <string>

Settings::Settings() :
  init_volume(50), min_volume(1), max_volume(100), volume_step(1)
{
}

void Settings::load()
{
  std::ifstream settings_file(settings_path);
  if (settings_file.is_open()) {
    std::string data(std::istreambuf_iterator<char>(settings_file), {});
    cJSON *json_root = cJSON_ParseWithLength(data.c_str(), data.length());
    if (json_root) {
      cJSON *json_init_volume = cJSON_GetObjectItem(json_root, "init_volume");
      if (cJSON_IsNumber(json_init_volume)) {
        init_volume = std::ranges::clamp(
            static_cast<int>(json_init_volume->valuedouble), 1, 100);
      }
      cJSON *json_min_volume = cJSON_GetObjectItem(json_root, "min_volume");
      if (cJSON_IsNumber(json_min_volume)) {
        min_volume = std::ranges::clamp(
            static_cast<int>(json_min_volume->valuedouble), 1, 100);
      }
      cJSON *json_max_volume = cJSON_GetObjectItem(json_root, "max_volume");
      if (cJSON_IsNumber(json_max_volume)) {
        max_volume = std::ranges::clamp(
            static_cast<int>(json_max_volume->valuedouble), 1, 100);
      }
      cJSON *json_volume_step = cJSON_GetObjectItem(json_root, "volume_step");
      if (cJSON_IsNumber(json_volume_step)) {
        volume_step = std::ranges::clamp(
            static_cast<int>(json_volume_step->valuedouble), 1, 5);
      }
      cJSON_Delete(json_root);
    }
  } else {
    std::ofstream new_file(settings_path);
    new_file << "{}";
  }
}

int Settings::get_init_volume()
{
  return init_volume;
}

int Settings::get_min_volume()
{
  return min_volume;
}

int Settings::get_max_volume()
{
  return max_volume;
}

int Settings::get_volume_step()
{
  return volume_step;
}
