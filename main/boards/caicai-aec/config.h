#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE         16000
#define AUDIO_OUTPUT_SAMPLE_RATE        16000

#define AUDIO_I2S_GPIO_MCLK             GPIO_NUM_40
#define AUDIO_I2S_GPIO_WS               GPIO_NUM_45
#define AUDIO_I2S_GPIO_BCLK             GPIO_NUM_39
#define AUDIO_I2S_GPIO_DIN              GPIO_NUM_21
#define AUDIO_I2S_GPIO_DOUT             GPIO_NUM_48

#define AUDIO_CODEC_PA_PIN              GPIO_NUM_46
#define AUDIO_CODEC_I2C_SDA_PIN         GPIO_NUM_38
#define AUDIO_CODEC_I2C_SCL_PIN         GPIO_NUM_47
#define AUDIO_CODEC_ES8311_ADDR         ES8311_CODEC_DEFAULT_ADDR
#define AUDIO_CODEC_ES7210_ADDR         ES7210_CODEC_DEFAULT_ADDR

#define IMU_I2C_SCL_PIN                 GPIO_NUM_15
#define IMU_I2C_SDA_PIN                 GPIO_NUM_7
#define IMU_INT_PIN                     GPIO_NUM_6
#define IMU_INT2_PIN                    GPIO_NUM_16

#define BUILTIN_LED_GPIO                GPIO_NUM_NC
#define BOOT_BUTTON_GPIO                GPIO_NUM_0
#define CHARGIN_DETECT_PIN              GPIO_NUM_41
#define VOLUME_UP_BUTTON_GPIO           GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO         GPIO_NUM_NC

#define DISPLAY_WIDTH                   360
#define DISPLAY_HEIGHT                  360
#define DISPLAY_MIRROR_X                false
#define DISPLAY_MIRROR_Y                false
#define DISPLAY_SWAP_XY                 false

#define DISPLAY_OFFSET_X                0
#define DISPLAY_OFFSET_Y                0

#define DISPLAY_BACKLIGHT_PIN           GPIO_NUM_17
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#define DISPLAY_QSPI_H_RES              (360)
#define DISPLAY_QSPI_V_RES              (360)
#define DISPLAY_QSPI_BIT_PER_PIXEL      (16)

#define DISPLAY_QSPI_HOST               SPI3_HOST
#define DISPLAY_QSPI_SCLK_PIN           GPIO_NUM_9
#define DISPLAY_QSPI_RESET_PIN          GPIO_NUM_8
#define DISPLAY_QSPI_D0_PIN             GPIO_NUM_11
#define DISPLAY_QSPI_D1_PIN             GPIO_NUM_12
#define DISPLAY_QSPI_D2_PIN             GPIO_NUM_13
#define DISPLAY_QSPI_D3_PIN             GPIO_NUM_10
#define DISPLAY_QSPI_CS_PIN             GPIO_NUM_18

#define DISPLAY_SPI_SCLK_HZ             (40 * 1000 * 1000)

#define MOJI2_ST77916_PANEL_BUS_QSPI_CONFIG(sclk, d0, d1, d2, d3, max_trans_sz) \
    {                                                                             \
        .data0_io_num = d0,                                                       \
        .data1_io_num = d1,                                                       \
        .sclk_io_num = sclk,                                                      \
        .data2_io_num = d2,                                                       \
        .data3_io_num = d3,                                                       \
        .max_transfer_sz = max_trans_sz,                                          \
    }

#define XCLK_FREQ_HZ                    20000000

#endif // _BOARD_CONFIG_H_
