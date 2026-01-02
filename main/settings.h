#pragma once

class Settings
{
public:
  Settings();
  void load();
  int get_init_volume();
  int get_min_volume();
  int get_max_volume();
  int get_volume_step();

private:
  static constexpr char settings_path[] = "/sd/settings.json";
  int init_volume;
  int min_volume;
  int max_volume;
  int volume_step;
};
