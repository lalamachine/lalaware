#include "i2c_bus.h"

#include <esp_err.h>
#include <soc/clk_tree_defs.h>

I2cBus::I2cBus(i2c_port_num_t port, gpio_num_t sda_pin, gpio_num_t scl_pin)
{
  i2c_master_bus_config_t config = {.i2c_port = port,
                                    .sda_io_num = sda_pin,
                                    .scl_io_num = scl_pin,
                                    .clk_source = I2C_CLK_SRC_DEFAULT,
                                    .glitch_ignore_cnt = 7,
                                    .flags = {.enable_internal_pullup = 1}};
  ESP_ERROR_CHECK(i2c_new_master_bus(&config, &handle));
}

void I2cBus::register_device(const i2c_device_config_t *device_config,
                             i2c_master_dev_handle_t *device_handle)
{
  ESP_ERROR_CHECK(
      i2c_master_bus_add_device(handle, device_config, device_handle));
}
