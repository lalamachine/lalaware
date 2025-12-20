#include <audio_error.h>
#include <board_pins_config.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <hal/i2c_types.h>
#include <soc/gpio_num.h>

static const char *TAG = "board";

esp_err_t get_i2c_pins(i2c_port_t port, i2c_config_t *i2c_config)
{
  AUDIO_NULL_CHECK(TAG, i2c_config, return ESP_FAIL);
  if (port == I2C_NUM_0) {
    i2c_config->sda_io_num = GPIO_NUM_33;
    i2c_config->scl_io_num = GPIO_NUM_32;
  } else {
    ESP_LOGE(TAG, "i2c port %d is not supported", port);
    return ESP_FAIL;
  }
  return ESP_OK;
}

esp_err_t get_i2s_pins(int port, board_i2s_pin_t *i2s_config)
{
  AUDIO_NULL_CHECK(TAG, i2s_config, return ESP_FAIL);
  if (port == 0) {
    i2s_config->mck_io_num = GPIO_NUM_0;
    i2s_config->bck_io_num = GPIO_NUM_27;
    i2s_config->ws_io_num = GPIO_NUM_25;
    i2s_config->data_out_num = GPIO_NUM_26;
    i2s_config->data_in_num = GPIO_NUM_35;
  } else {
    ESP_LOGE(TAG, "i2s port %d is not supported", port);
    return ESP_FAIL;
  }
  return ESP_OK;
}

int8_t get_sdcard_intr_gpio(void)
{
  return GPIO_NUM_34;
}

int8_t get_sdcard_open_file_num_max(void)
{
  return 2;
}

int8_t get_headphone_detect_gpio(void)
{
  return GPIO_NUM_39;
}

int8_t get_pa_enable_gpio(void)
{
  return GPIO_NUM_21;
}
