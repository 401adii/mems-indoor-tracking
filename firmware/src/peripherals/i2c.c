#include "i2c.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include <stdint.h>

static i2c_master_bus_handle_t g_bus_handle;
static i2c_master_dev_handle_t g_dev_handles[I2C_DEVICE_NUM];

void i2c_init_bus()
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = 1,
    };
    i2c_new_master_bus(&bus_config, &g_bus_handle);
}

void i2c_add_device(i2c_device_t device, uint8_t address)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = address,
        .scl_speed_hz = 100000,
        .scl_wait_us = 10000
    };
    i2c_master_bus_add_device(g_bus_handle, &dev_config, &g_dev_handles[device]);
}