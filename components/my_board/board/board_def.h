#pragma once

/**
 * @brief SDCARD Function Definition
 */
#define FUNC_SDCARD_EN (1)
#define SDCARD_OPEN_FILE_NUM_MAX 5
#define SDCARD_INTR_GPIO GPIO_NUM_34

#define ESP_SD_PIN_CLK GPIO_NUM_14
#define ESP_SD_PIN_CMD GPIO_NUM_15
#define ESP_SD_PIN_D0 GPIO_NUM_2
#define ESP_SD_PIN_D1 GPIO_NUM_4
#define ESP_SD_PIN_D2 GPIO_NUM_12
#define ESP_SD_PIN_D3 GPIO_NUM_13

/**
 * @brief Audio Codec Chip Function Definition
 */
#define FUNC_AUDIO_CODEC_EN (1)
#define HEADPHONE_DETECT GPIO_NUM_39
#define PA_ENABLE_GPIO GPIO_NUM_21
#define CODEC_ADC_I2S_PORT ((i2s_port_t) 0)
#define CODEC_ADC_BITS_PER_SAMPLE ((i2s_data_bit_width_t) 16) /* 16bit */
#define CODEC_ADC_SAMPLE_RATE (48000)
#define RECORD_HARDWARE_AEC (false)
#define BOARD_PA_GAIN (10) /* Power amplifier gain defined by board (dB) */
