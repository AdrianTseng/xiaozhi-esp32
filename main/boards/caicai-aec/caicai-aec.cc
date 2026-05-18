#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "codecs/box_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "i2c_device.h"
#include "mcp_server.h"
#include "power_save_timer.h"
#include "adc_battery_monitor.h"
#include "sleep_timer.h"
#include "bmi270_api.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_st77916.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>
#include <esp_sleep.h>


#define TAG "CaiCaiAECBoard"

static const st77916_lcd_init_cmd_t lcd_init_cmds[] = {
    {0xF0, (uint8_t []){0x28}, 1, 0},
    {0xF2, (uint8_t []){0x28}, 1, 0},
    {0x73, (uint8_t []){0xF0}, 1, 0},
    {0x7C, (uint8_t []){0xD1}, 1, 0},
    {0x83, (uint8_t []){0xE0}, 1, 0},
    {0x84, (uint8_t []){0x61}, 1, 0},
    {0xF2, (uint8_t []){0x82}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0xF0, (uint8_t []){0x01}, 1, 0},
    {0xF1, (uint8_t []){0x01}, 1, 0},
    {0xB0, (uint8_t []){0x56}, 1, 0},
    {0xB1, (uint8_t []){0x4D}, 1, 0},
    {0xB2, (uint8_t []){0x24}, 1, 0},
    {0xB4, (uint8_t []){0x87}, 1, 0},
    {0xB5, (uint8_t []){0x44}, 1, 0},
    {0xB6, (uint8_t []){0x8B}, 1, 0},
    {0xB7, (uint8_t []){0x40}, 1, 0},
    {0xB8, (uint8_t []){0x86}, 1, 0},
    {0xBA, (uint8_t []){0x00}, 1, 0},
    {0xBB, (uint8_t []){0x08}, 1, 0},
    {0xBC, (uint8_t []){0x08}, 1, 0},
    {0xBD, (uint8_t []){0x00}, 1, 0},
    {0xC0, (uint8_t []){0x80}, 1, 0},
    {0xC1, (uint8_t []){0x10}, 1, 0},
    {0xC2, (uint8_t []){0x37}, 1, 0},
    {0xC3, (uint8_t []){0x80}, 1, 0},
    {0xC4, (uint8_t []){0x10}, 1, 0},
    {0xC5, (uint8_t []){0x37}, 1, 0},
    {0xC6, (uint8_t []){0xA9}, 1, 0},
    {0xC7, (uint8_t []){0x41}, 1, 0},
    {0xC8, (uint8_t []){0x01}, 1, 0},
    {0xC9, (uint8_t []){0xA9}, 1, 0},
    {0xCA, (uint8_t []){0x41}, 1, 0},
    {0xCB, (uint8_t []){0x01}, 1, 0},
    {0xD0, (uint8_t []){0x91}, 1, 0},
    {0xD1, (uint8_t []){0x68}, 1, 0},
    {0xD2, (uint8_t []){0x68}, 1, 0},
    {0xF5, (uint8_t []){0x00, 0xA5}, 2, 0},
    {0xDD, (uint8_t []){0x4F}, 1, 0},
    {0xDE, (uint8_t []){0x4F}, 1, 0},
    {0xF1, (uint8_t []){0x10}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0xF0, (uint8_t []){0x02}, 1, 0},
    {0xE0, (uint8_t []){0xF0, 0x0A, 0x10, 0x09, 0x09, 0x36, 0x35, 0x33, 0x4A, 0x29, 0x15, 0x15, 0x2E, 0x34}, 14, 0},
    {0xE1, (uint8_t []){0xF0, 0x0A, 0x0F, 0x08, 0x08, 0x05, 0x34, 0x33, 0x4A, 0x39, 0x15, 0x15, 0x2D, 0x33}, 14, 0},
    {0xF0, (uint8_t []){0x10}, 1, 0},
    {0xF3, (uint8_t []){0x10}, 1, 0},
    {0xE0, (uint8_t []){0x07}, 1, 0},
    {0xE1, (uint8_t []){0x00}, 1, 0},
    {0xE2, (uint8_t []){0x00}, 1, 0},
    {0xE3, (uint8_t []){0x00}, 1, 0},
    {0xE4, (uint8_t []){0xE0}, 1, 0},
    {0xE5, (uint8_t []){0x06}, 1, 0},
    {0xE6, (uint8_t []){0x21}, 1, 0},
    {0xE7, (uint8_t []){0x01}, 1, 0},
    {0xE8, (uint8_t []){0x05}, 1, 0},
    {0xE9, (uint8_t []){0x02}, 1, 0},
    {0xEA, (uint8_t []){0xDA}, 1, 0},
    {0xEB, (uint8_t []){0x00}, 1, 0},
    {0xEC, (uint8_t []){0x00}, 1, 0},
    {0xED, (uint8_t []){0x0F}, 1, 0},
    {0xEE, (uint8_t []){0x00}, 1, 0},
    {0xEF, (uint8_t []){0x00}, 1, 0},
    {0xF8, (uint8_t []){0x00}, 1, 0},
    {0xF9, (uint8_t []){0x00}, 1, 0},
    {0xFA, (uint8_t []){0x00}, 1, 0},
    {0xFB, (uint8_t []){0x00}, 1, 0},
    {0xFC, (uint8_t []){0x00}, 1, 0},
    {0xFD, (uint8_t []){0x00}, 1, 0},
    {0xFE, (uint8_t []){0x00}, 1, 0},
    {0xFF, (uint8_t []){0x00}, 1, 0},
    {0x60, (uint8_t []){0x40}, 1, 0},
    {0x61, (uint8_t []){0x04}, 1, 0},
    {0x62, (uint8_t []){0x00}, 1, 0},
    {0x63, (uint8_t []){0x42}, 1, 0},
    {0x64, (uint8_t []){0xD9}, 1, 0},
    {0x65, (uint8_t []){0x00}, 1, 0},
    {0x66, (uint8_t []){0x00}, 1, 0},
    {0x67, (uint8_t []){0x00}, 1, 0},
    {0x68, (uint8_t []){0x00}, 1, 0},
    {0x69, (uint8_t []){0x00}, 1, 0},
    {0x6A, (uint8_t []){0x00}, 1, 0},
    {0x6B, (uint8_t []){0x00}, 1, 0},
    {0x70, (uint8_t []){0x40}, 1, 0},
    {0x71, (uint8_t []){0x03}, 1, 0},
    {0x72, (uint8_t []){0x00}, 1, 0},
    {0x73, (uint8_t []){0x42}, 1, 0},
    {0x74, (uint8_t []){0xD8}, 1, 0},
    {0x75, (uint8_t []){0x00}, 1, 0},
    {0x76, (uint8_t []){0x00}, 1, 0},
    {0x77, (uint8_t []){0x00}, 1, 0},
    {0x78, (uint8_t []){0x00}, 1, 0},
    {0x79, (uint8_t []){0x00}, 1, 0},
    {0x7A, (uint8_t []){0x00}, 1, 0},
    {0x7B, (uint8_t []){0x00}, 1, 0},
    {0x80, (uint8_t []){0x48}, 1, 0},
    {0x81, (uint8_t []){0x00}, 1, 0},
    {0x82, (uint8_t []){0x06}, 1, 0},
    {0x83, (uint8_t []){0x02}, 1, 0},
    {0x84, (uint8_t []){0xD6}, 1, 0},
    {0x85, (uint8_t []){0x04}, 1, 0},
    {0x86, (uint8_t []){0x00}, 1, 0},
    {0x87, (uint8_t []){0x00}, 1, 0},
    {0x88, (uint8_t []){0x48}, 1, 0},
    {0x89, (uint8_t []){0x00}, 1, 0},
    {0x8A, (uint8_t []){0x08}, 1, 0},
    {0x8B, (uint8_t []){0x02}, 1, 0},
    {0x8C, (uint8_t []){0xD8}, 1, 0},
    {0x8D, (uint8_t []){0x04}, 1, 0},
    {0x8E, (uint8_t []){0x00}, 1, 0},
    {0x8F, (uint8_t []){0x00}, 1, 0},
    {0x90, (uint8_t []){0x48}, 1, 0},
    {0x91, (uint8_t []){0x00}, 1, 0},
    {0x92, (uint8_t []){0x0A}, 1, 0},
    {0x93, (uint8_t []){0x02}, 1, 0},
    {0x94, (uint8_t []){0xDA}, 1, 0},
    {0x95, (uint8_t []){0x04}, 1, 0},
    {0x96, (uint8_t []){0x00}, 1, 0},
    {0x97, (uint8_t []){0x00}, 1, 0},
    {0x98, (uint8_t []){0x48}, 1, 0},
    {0x99, (uint8_t []){0x00}, 1, 0},
    {0x9A, (uint8_t []){0x0C}, 1, 0},
    {0x9B, (uint8_t []){0x02}, 1, 0},
    {0x9C, (uint8_t []){0xDC}, 1, 0},
    {0x9D, (uint8_t []){0x04}, 1, 0},
    {0x9E, (uint8_t []){0x00}, 1, 0},
    {0x9F, (uint8_t []){0x00}, 1, 0},
    {0xA0, (uint8_t []){0x48}, 1, 0},
    {0xA1, (uint8_t []){0x00}, 1, 0},
    {0xA2, (uint8_t []){0x05}, 1, 0},
    {0xA3, (uint8_t []){0x02}, 1, 0},
    {0xA4, (uint8_t []){0xD5}, 1, 0},
    {0xA5, (uint8_t []){0x04}, 1, 0},
    {0xA6, (uint8_t []){0x00}, 1, 0},
    {0xA7, (uint8_t []){0x00}, 1, 0},
    {0xA8, (uint8_t []){0x48}, 1, 0},
    {0xA9, (uint8_t []){0x00}, 1, 0},
    {0xAA, (uint8_t []){0x07}, 1, 0},
    {0xAB, (uint8_t []){0x02}, 1, 0},
    {0xAC, (uint8_t []){0xD7}, 1, 0},
    {0xAD, (uint8_t []){0x04}, 1, 0},
    {0xAE, (uint8_t []){0x00}, 1, 0},
    {0xAF, (uint8_t []){0x00}, 1, 0},
    {0xB0, (uint8_t []){0x48}, 1, 0},
    {0xB1, (uint8_t []){0x00}, 1, 0},
    {0xB2, (uint8_t []){0x09}, 1, 0},
    {0xB3, (uint8_t []){0x02}, 1, 0},
    {0xB4, (uint8_t []){0xD9}, 1, 0},
    {0xB5, (uint8_t []){0x04}, 1, 0},
    {0xB6, (uint8_t []){0x00}, 1, 0},
    {0xB7, (uint8_t []){0x00}, 1, 0},
    {0xB8, (uint8_t []){0x48}, 1, 0},
    {0xB9, (uint8_t []){0x00}, 1, 0},
    {0xBA, (uint8_t []){0x0B}, 1, 0},
    {0xBB, (uint8_t []){0x02}, 1, 0},
    {0xBC, (uint8_t []){0xDB}, 1, 0},
    {0xBD, (uint8_t []){0x04}, 1, 0},
    {0xBE, (uint8_t []){0x00}, 1, 0},
    {0xBF, (uint8_t []){0x00}, 1, 0},
    {0xC0, (uint8_t []){0x10}, 1, 0},
    {0xC1, (uint8_t []){0x47}, 1, 0},
    {0xC2, (uint8_t []){0x56}, 1, 0},
    {0xC3, (uint8_t []){0x65}, 1, 0},
    {0xC4, (uint8_t []){0x74}, 1, 0},
    {0xC5, (uint8_t []){0x88}, 1, 0},
    {0xC6, (uint8_t []){0x99}, 1, 0},
    {0xC7, (uint8_t []){0x01}, 1, 0},
    {0xC8, (uint8_t []){0xBB}, 1, 0},
    {0xC9, (uint8_t []){0xAA}, 1, 0},
    {0xD0, (uint8_t []){0x10}, 1, 0},
    {0xD1, (uint8_t []){0x47}, 1, 0},
    {0xD2, (uint8_t []){0x56}, 1, 0},
    {0xD3, (uint8_t []){0x65}, 1, 0},
    {0xD4, (uint8_t []){0x74}, 1, 0},
    {0xD5, (uint8_t []){0x88}, 1, 0},
    {0xD6, (uint8_t []){0x99}, 1, 0},
    {0xD7, (uint8_t []){0x01}, 1, 0},
    {0xD8, (uint8_t []){0xBB}, 1, 0},
    {0xD9, (uint8_t []){0xAA}, 1, 0},
    {0xF3, (uint8_t []){0x01}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0x21, (uint8_t []){}, 0, 0},
    {0x11, (uint8_t []){}, 0, 0},
    {0x00, (uint8_t []){}, 0, 120},
};


class CaiCaiAECBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    i2c_bus_handle_t imu_i2c_bus_;
    bmi270_handle_t imu_dev_;
    Button boot_button_, any_motion_button_;
    Display* display_;
    PowerSaveTimer* power_save_timer_ = nullptr;
    AdcBatteryMonitor* adc_battery_monitor_ = nullptr;
    int64_t last_wakeup_time_ = 0;

    void InitializeBatteryMonitor() {
        // 后面可以更改这里
        adc_battery_monitor_ = new AdcBatteryMonitor(ADC_UNIT_1, ADC_CHANNEL_2, 200000, 100000, CHARGIN_DETECT_PIN);
        adc_battery_monitor_->OnChargingStatusChanged([this](bool is_charging) {
            if (power_save_timer_ != nullptr){
                if (is_charging) {
                    power_save_timer_->SetEnabled(false);
                } else {
                    power_save_timer_->SetEnabled(true);
                }
            }

        });
    }

    void InitializePowerSaveTimer() {
        power_save_timer_ = new PowerSaveTimer(240, 45, 180);
        power_save_timer_->OnEnterSleepMode([this]() {
            GetDisplay()->SetPowerSaveMode(true);
            GetBacklight()->RestoreBrightness();
            bmi2_set_adv_power_save(BMI2_ENABLE, imu_dev_);
        });
        power_save_timer_->OnExitSleepMode([this]() {
            GetDisplay()->SetPowerSaveMode(false);
            GetBacklight()->SetBrightness(0, false);
            bmi2_set_adv_power_save(BMI2_DISABLE, imu_dev_);
        });
        power_save_timer_->OnShutdownRequest([this]() {
            auto& app = Application::GetInstance();
            GetDisplay()->SetPowerSaveMode(true);
            while(!app.CanEnterSleepMode()){
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            GetBacklight()->SetBrightness(0, false);
            ESP_LOGI(TAG, "Shuting down...");
            vTaskDelay(pdMS_TO_TICKS(100));
            esp_sleep_enable_ext0_wakeup(IMU_INT2_PIN, 1);
            esp_deep_sleep_start();
        });
        power_save_timer_->SetEnabled(true);
    }

    void InitializeI2c() {
        // Initialize I2C peripheral
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = (i2c_port_t)I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
    }

    void InitializeSpi() {
        const spi_bus_config_t screen_bus_config = MOJI2_ST77916_PANEL_BUS_QSPI_CONFIG(DISPLAY_QSPI_SCLK_PIN,
                                                                                    DISPLAY_QSPI_D0_PIN,
                                                                                    DISPLAY_QSPI_D1_PIN,
                                                                                    DISPLAY_QSPI_D2_PIN,
                                                                                    DISPLAY_QSPI_D3_PIN,
                                                                                    DISPLAY_QSPI_H_RES * 80 * sizeof(uint16_t));
        ESP_ERROR_CHECK(spi_bus_initialize(DISPLAY_QSPI_HOST, &screen_bus_config, SPI_DMA_CH_AUTO));
    }

    void InitializeIMU(){
        const i2c_config_t i2c_bus_conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = IMU_I2C_SDA_PIN,
            .scl_io_num = IMU_I2C_SCL_PIN,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master = {
                .clk_speed = 100000,
            }
        };

        imu_i2c_bus_ = i2c_bus_create(I2C_NUM_1, &i2c_bus_conf);
        ESP_ERROR_CHECK(bmi270_sensor_create(imu_i2c_bus_, &imu_dev_, bmi270_toy_config_file, 0));

        int8_t rslt;
        struct bmi2_sens_config config[2];
        struct bmi2_int_pin_config pin_config = { 0 };

        config[BMI2_ACCEL].type = BMI2_ACCEL;
        config[BMI2_GYRO].type = BMI2_GYRO;

        bmi2_soft_reset(imu_dev_);

        rslt = bmi2_get_int_pin_config(&pin_config, imu_dev_);
        bmi2_error_codes_print_result(rslt);

        rslt = bmi2_get_sensor_config(config, 2, imu_dev_);
        bmi2_error_codes_print_result(rslt);

        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to get sensor config");
            return;
        }

        /* Configure accelerometer output data rate */
        config[BMI2_ACCEL].cfg.acc.odr = BMI2_ACC_ODR_100HZ;        /* 100Hz sampling rate */
        config[BMI2_ACCEL].cfg.acc.range = BMI2_ACC_RANGE_4G;      /* ±4G range */
        config[BMI2_ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;      /* Standard averaging */
        config[BMI2_ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE; /* Filter performance */

        /* Configure gyroscope output data rate */
        config[BMI2_GYRO].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;         /* 100Hz sampling rate */
        config[BMI2_GYRO].cfg.gyr.range = BMI2_GYR_RANGE_500;      /* ±500dps range */
        config[BMI2_GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;       /* Standard filtering */
        config[BMI2_GYRO].cfg.gyr.noise_perf = BMI2_PERF_OPT_MODE;  /* Noise performance */
        config[BMI2_GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE; /* Filter performance */

        rslt = bmi2_set_sensor_config(config, 2, imu_dev_);
        bmi2_error_codes_print_result(rslt);
        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to set accel & gyro configs.");
            return;
        }

        pin_config.pin_type = BMI2_INT2;
        pin_config.pin_cfg[1].input_en = BMI2_INT_INPUT_DISABLE;
        pin_config.pin_cfg[1].lvl = BMI2_INT_ACTIVE_HIGH; // High level trigger as requested
        pin_config.pin_cfg[1].od = BMI2_INT_PUSH_PULL;
        pin_config.pin_cfg[1].output_en = BMI2_INT_OUTPUT_ENABLE;
        pin_config.int_latch = BMI2_INT_NON_LATCH; // Non-latched so it goes low after event is cleared or read

        rslt = bmi2_set_int_pin_config(&pin_config, imu_dev_);
        bmi2_error_codes_print_result(rslt);
        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to set interrupt pin config.");
            return;
        }

        // Map Any-Motion interrupt to INT2
        uint8_t data = BMI270_TOY_INT_ANY_MOT_MASK;
        // BMI2_INT2_MAP_FEAT_ADDR is typically 0x58 for INT2 feature mapping in BMI270
        rslt = bmi2_set_regs(BMI2_INT2_MAP_FEAT_ADDR, &data, 1, imu_dev_);
        bmi2_error_codes_print_result(rslt);
        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to set interrupt feature pin-map config.");
            return;
        }

        uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };
        // Enable sensors
        rslt = bmi2_sensor_enable(sens_list, 2, imu_dev_);
        bmi2_error_codes_print_result(rslt);
        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to enable sensors.");
            return;
        }

        // Enable any-motion feature
        rslt = bmi270_enable_toy_any_motion(imu_dev_, BMI2_ENABLE);
        bmi2_error_codes_print_result(rslt);
        if(rslt != BMI2_OK){
            ESP_LOGE(TAG, "Failed to enable any-motion feature.");
            return;
        }
        else{
            ESP_LOGI(TAG, "Any-motion feature enabled, result: %d", rslt);
        }
    }

    
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            // During startup (before connected), pressing BOOT button enters Wi-Fi config mode without reboot
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });

        any_motion_button_.OnPressDown([this](){
            uint8_t int_status;
            bmi2_get_regs(BMI2_INT_STATUS_0_ADDR, &int_status, 1, imu_dev_);
            if(int_status & BMI270_TOY_INT_ANY_MOT_MASK){
                int64_t current_time = esp_timer_get_time();
                if(current_time - last_wakeup_time_> 15000000){
                    ESP_LOGI(TAG, "Wake up by any motion.");
                    power_save_timer_ -> WakeUp();
                    last_wakeup_time_ = current_time;
                }
            }
        });
        // any_motion_button_.OnClick([this]() {
        //     ESP_LOGI(TAG, "motion detected.");
        //     power_save_timer_->WakeUp();
        // });

#if CONFIG_USE_DEVICE_AEC
        boot_button_.OnDoubleClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateIdle) {
                app.SetAecMode(app.GetAecMode() == kAecOff ? kAecOnDeviceSide : kAecOff);
            }
            app.PlaySound("aec_toggle");
        });
#endif
    }

    // St77916 初始化
    void InitializeSt77916Display() {
        ESP_LOGI(TAG, "Init St77916 display");
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        ESP_LOGI(TAG, "Install panel IO");
        
        esp_lcd_panel_io_spi_config_t io_config = ST77916_PANEL_IO_QSPI_CONFIG(DISPLAY_QSPI_CS_PIN, NULL, NULL);
        // io_config.pclk_hz = DISPLAY_SPI_SCLK_HZ;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_QSPI_HOST, &io_config, &panel_io));

    
        ESP_LOGI(TAG, "Install St77916 panel driver");
        st77916_vendor_config_t vendor_config = {
            .init_cmds = lcd_init_cmds,
            .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(st77916_lcd_init_cmd_t),
            .flags = {
                .use_qspi_interface = 1,
            },
        };
        const esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = DISPLAY_QSPI_RESET_PIN,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = DISPLAY_QSPI_BIT_PER_PIXEL,
            .vendor_config = &vendor_config,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_st77916(panel_io, &panel_config, &panel));

        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_disp_on_off(panel, true);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);

        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }   


    void InitializeTools() {
        auto &mcp_server = McpServer::GetInstance();
        mcp_server.AddTool("self.system.reconfigure_wifi",
            "End this conversation and enter WiFi configuration mode.\n"
            "**CAUTION** You must ask the user to confirm this action.",
            PropertyList(), [this](const PropertyList& properties) {
                EnterWifiConfigMode();
                return true;
            });
    }

public:
    CaiCaiAECBoard() : boot_button_(BOOT_BUTTON_GPIO), any_motion_button_(IMU_INT2_PIN, true) {
        InitializeI2c();
        InitializeSpi();
        InitializeBatteryMonitor();
        InitializePowerSaveTimer();
        InitializeIMU();
        InitializeSt77916Display();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->RestoreBrightness();
    }

    // virtual Led* GetLed() override {
    //     static SingleLed led_strip(BUILTIN_LED_GPIO);
    //     return &led_strip;
    // }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }

    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(codec_i2c_bus_, I2C_NUM_0, AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    virtual bool GetBatteryLevel(int& level, bool& charging, bool& discharging) override {
        charging = adc_battery_monitor_->IsCharging();
        discharging = adc_battery_monitor_->IsDischarging();
        level = adc_battery_monitor_->GetBatteryLevel();
        return true;
    }
};

DECLARE_BOARD(CaiCaiAECBoard);
