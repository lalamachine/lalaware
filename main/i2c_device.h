#pragma once

#include "i2c_bus.h"

#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <driver/i2c_master.h>
#include <driver/i2c_types.h>
#include <vector>

class I2cDevice
{
protected:
  I2cDevice(I2cBus &bus, i2c_device_config_t config);
  bool transmit(const uint8_t *write_buffer, size_t write_size,
                int xfer_timeout_ms);
  bool transmit(const std::vector<uint8_t> &write_buffer, int xfer_timeout_ms);
  bool receive(uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);
  bool receive(std::vector<uint8_t> &read_buffer, int xfer_timeout_ms);

private:
  i2c_master_dev_handle_t handle;
};
