#include "i2c_device.h"

#include <esp_err.h>

I2cDevice::I2cDevice(I2cBus &bus, i2c_device_config_t config)
{
  bus.register_device(&config, &handle);
}

bool I2cDevice::transmit(const uint8_t *write_buffer, size_t write_size,
                         int xfer_timeout_ms)
{
  if (i2c_master_transmit(handle, write_buffer, write_size, xfer_timeout_ms) ==
      ESP_OK) {
    return true;
  }
  return false;
}

bool I2cDevice::transmit(const std::vector<uint8_t> &write_buffer,
                         int xfer_timeout_ms)
{
  return transmit(write_buffer.data(), write_buffer.size(), xfer_timeout_ms);
}

bool I2cDevice::receive(uint8_t *read_buffer, size_t read_size,
                        int xfer_timeout_ms)
{
  if (i2c_master_receive(handle, read_buffer, read_size, xfer_timeout_ms) ==
      ESP_OK) {
    return true;
  }
  return false;
}

bool I2cDevice::receive(std::vector<uint8_t> &read_buffer, int xfer_timeout_ms)
{
  return receive(read_buffer.data(), read_buffer.size(), xfer_timeout_ms);
}
