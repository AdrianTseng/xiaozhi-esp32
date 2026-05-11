#pragma once

#include <driver/spi_common.h>
#include <driver/gpio.h>
#include <functional>
#include <mutex>

class Bmi270Imu {
public:
    enum InterruptType {
        INTERRUPT_ANY_MOTION = 0x01,
        INTERRUPT_SINGLE_TAP = 0x02,
        INTERRUPT_DOUBLE_TAP = 0x04
    };

    // 回调函数类型
    using InterruptCallback = std::function<void(InterruptType type)>;

    Bmi270Imu(spi_host_device_t spi_host, gpio_num_t cs_pin, gpio_num_t int_pin, gpio_num_t int2_pin);
    ~Bmi270Imu();

    /**
     * @brief 初始化 BMI270
     * @return true: 初始化成功, false: 初始化失败
     */
    bool Initialize();

    /**
     * @brief 设置中断回调函数
     * @param callback 中断回调函数
     */
    void SetInterruptCallback(InterruptCallback callback);

    /**
     * @brief 启用/禁用中断类型
     * @param type 中断类型
     * @param enable true: 启用, false: 禁用
     * @return true: 操作成功, false: 操作失败
     */
    bool EnableInterrupt(InterruptType type, bool enable);

    /**
     * @brief 设置加速度计范围
     * @param range 范围值 (2, 4, 8, 16 对应±2g, ±4g, ±8g, ±16g)
     * @return true: 操作成功, false: 操作失败
     */
    bool SetAccelerometerRange(uint8_t range);

    /**
     * @brief 设置任意运动检测参数
     * @param duration 持续时间 (单位: ms)
     * @param threshold 阈值
     * @return true: 操作成功, false: 操作失败
     */
    bool SetAnyMotionConfig(uint16_t duration, uint8_t threshold);

    /**
     * @brief 设置敲击检测参数
     * @param single_tap_enable 启用单击检测
     * @param double_tap_enable 启用双击检测
     * @param sensitivity 灵敏度 (0-3, 0为最灵敏)
     * @return true: 操作成功, false: 操作失败
     */
    bool SetTapConfig(bool single_tap_enable, bool double_tap_enable, uint8_t sensitivity);

    /**
     * @brief 获取芯片ID
     * @return 芯片ID
     */
    uint8_t GetChipId() const { return chip_id_; }

    /**
     * @brief 检查设备是否初始化成功
     * @return true: 已初始化, false: 未初始化
     */
    bool IsInitialized() const { return initialized_; }

private:
    // 设备句柄结构 (在 .cpp 文件中定义具体实现)
    struct Bmi270Device;

    // SPI 相关
    spi_host_device_t spi_host_;
    gpio_num_t cs_pin_;
    gpio_num_t int1_pin_;
    gpio_num_t int2_pin_;
    spi_device_handle_t spi_device_;

     // BMI270 设备
    Bmi270Device* bmi270_dev_;
    uint8_t chip_id_;
    bool initialized_;

     // 回调函数
    InterruptCallback interrupt_callback_;
    std::mutex callback_mutex_;
    // 任务句柄
    TaskHandle_t interrupt_task_handle_;

    // 中断状态
    volatile uint8_t interrupt_flags_;

    // 私有方法
    bool SetupSpi();
    bool SetupInterrupts();
    bool ConfigureInterruptPins();
    bool ConfigureSensors();
    static void InterruptTask(void* arg);
    static void ISRHandlerInt1(void* arg);
    static void ISRHandlerInt2(void* arg);
    
    // 静态回调函数供底层驱动调用
    static int8_t SpiRead(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr);
    static int8_t SpiWrite(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr);
    static void DelayUs(uint32_t period, void* intf_ptr);
    
    // 禁用复制和赋值
    Bmi270Imu(const Bmi270Imu&) = delete;
    Bmi270Imu& operator=(const Bmi270Imu&) = delete;
}