#include "nfc_monitor.h"

#include "i2c_device.h"
#include "period.h"

#include <algorithm>
#include <cstdbool>
#include <cstdint>
#include <numeric>
#include <thread>
#include <unistd.h>
#include <vector>

NfcMonitor::NfcMonitor(I2cBus &i2c_bus) :
  I2cDevice(i2c_bus, {.dev_addr_length = I2C_ADDR_BIT_LEN_7,
                      .device_address = 0x24,
                      .scl_speed_hz = 100000,
                      .scl_wait_us = 200000,
                      .flags = {}}),
  status{}
{
}

void NfcMonitor::start()
{
  thread = std::thread(&NfcMonitor::entry, this);
}

NfcMonitor::Status NfcMonitor::get_status()
{
  std::lock_guard<std::mutex> guard(mutex);
  return status;
}

void NfcMonitor::entry()
{
  if (initialize()) {
    Period period(250);
    while (true) {
      period.wait_next();
      std::vector<uint8_t> uid;
      if (read_uid(uid)) {
        std::lock_guard<std::mutex> guard(mutex);
        status.present = true;
        std::copy_n(uid.begin(), 4, status.uid.begin());
      } else {
        std::lock_guard<std::mutex> guard(mutex);
        status.present = false;
        std::fill_n(status.uid.begin(), 4, 0);
      }
    }
  }
}

bool NfcMonitor::initialize()
{
  static std::vector<uint8_t> command = {0x14, 0x01, 0x00, 0x00};
  std::vector<uint8_t> response(1);
  if (!do_command(command, response)) {
    return false;
  }
  if (response[0] != 0x15) {
    return false;
  }
  return true;
}

bool NfcMonitor::read_uid(std::vector<uint8_t> &uid)
{
  static std::vector<uint8_t> command = {0x4A, 0x01, 0x00};
  std::vector<uint8_t> response(11);
  if (!do_command(command, response)) {
    return false;
  }
  if (response[0] != 0x4B) {
    return false;
  }
  if (response[1] != 0x01) {
    return false;
  }
  if (response[6] != 0x04) {
    return false;
  }
  uid.insert(uid.begin(), response.begin() + 7, response.begin() + 11);
  return true;
}

bool NfcMonitor::do_command(const std::vector<uint8_t> &command,
                            std::vector<uint8_t> &response)
{
  return send_command(command) && wait_ack() && read_response(response);
}

bool NfcMonitor::send_command(const std::vector<uint8_t> &command)
{
  uint8_t len = command.size() + 1;
  uint8_t lcs = ~len + 1;
  std::vector<uint8_t> message = {0x00, 0x00, 0xFF, len, lcs, 0xD4};
  message.insert(message.end(), command.begin(), command.end());
  uint8_t sum = std::accumulate(message.begin() + 5, message.end(), 0);
  uint8_t dcs = ~sum + 1;
  message.push_back(dcs);
  message.push_back(0x00);
  return transmit(message, 20);
}

bool NfcMonitor::wait_ack()
{
  static std::vector<uint8_t> ack = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
  std::vector<uint8_t> data(6);
  if (!poll_data(data)) {
    return false;
  }
  if (data != ack) {
    return false;
  }
  return true;
}

bool NfcMonitor::read_response(std::vector<uint8_t> &response)
{
  std::vector<uint8_t> data(response.size() + 8);
  if (!poll_data(data)) {
    return false;
  }
  if (data[3] != response.size() + 1) {
    return false;
  }
  if (data[5] != 0xD5) {
    return false;
  }
  std::copy(data.begin() + 6, data.end() - 2, response.begin());
  return true;
}

bool NfcMonitor::poll_data(std::vector<uint8_t> &data)
{
  Period period(20);
  for (uint32_t i = 0; i < 10; i++) {
    period.wait_next();
    std::vector<uint8_t> message(data.size() + 1);
    if (receive(message, 20)) {
      if (message[0] == 0x01) {
        std::copy(message.begin() + 1, message.end(), data.begin());
        return true;
      }
    }
  }
  return false;
}
