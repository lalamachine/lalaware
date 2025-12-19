#pragma once

#include <driver/i2c_master.h>
#include <soc/gpio_num.h>

class I2cBus
{
public:
  I2cBus(i2c_port_num_t port, gpio_num_t sda_pin, gpio_num_t scl_pin);
  void register_device(const i2c_device_config_t *device_config,
                       i2c_master_dev_handle_t *device_handle);

private:
  i2c_master_bus_handle_t handle;
};
