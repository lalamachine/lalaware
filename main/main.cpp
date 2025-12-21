#include "player.h"

#include <bootloader_random.h>

Player player;

extern "C" void app_main(void)
{
  bootloader_random_enable();
  player.run();
}
