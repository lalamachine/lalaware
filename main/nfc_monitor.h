#pragma once

#include "i2c_bus.h"
#include "i2c_device.h"

#include <array>
#include <cstdbool>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

class NfcMonitor : I2cDevice
{
public:
  struct Status
  {
    bool present;
    std::array<uint8_t, 4> uid;
  };
  NfcMonitor(I2cBus &i2c_bus);
  void start();
  Status get_status();

private:
  Status status;
  std::thread thread;
  std::mutex mutex;
  void entry();
  bool initialize();
  bool read_uid(std::vector<uint8_t> &uid);
  bool do_command(const std::vector<uint8_t> &command,
                  std::vector<uint8_t> &response);
  bool send_command(const std::vector<uint8_t> &command);
  bool wait_ack();
  bool read_response(std::vector<uint8_t> &response);
  bool poll_data(std::vector<uint8_t> &data);
};
