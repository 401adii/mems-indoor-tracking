#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "lsm6dsox.h"

#define MAX_DELAY pdMS_TO_TICKS(1000)

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;
lsm6dsox_t lsm6dsox = {0};

static void i2c_transmit(uint8_t *buffer, uint16_t size)
{
    i2c_master_transmit(dev_handle, buffer, size, MAX_DELAY);
}

static void i2c_receive(uint8_t *buffer, uint16_t size)
{
    i2c_master_receive(dev_handle, buffer, size, MAX_DELAY);
}

void app_main()
{
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .i2c_port = I2C_NUM_0,
        .flags.enable_internal_pullup = 1,
        .glitch_ignore_cnt = 7,
    };
    i2c_new_master_bus(&bus_config, &bus_handle);

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = LSM6DSOX_I2C_ADDRESS,
        .scl_speed_hz = 100000,
        .scl_wait_us = 10000,
        };
    i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);

    lsm6dsox.transmit = i2c_transmit;
    lsm6dsox.receive = i2c_receive;

    if(lsm6dsox_init(&lsm6dsox) == LSM6DSOX_OK)
    {
        printf("OK\n");
    }
    else
    {
        printf("ERROR\n");
        return;
    }

    if(lsm6dsox_enable_accel(&lsm6dsox, LSM6DSOX_ACCEL_DEFAULT_CONFIG) == LSM6DSOX_OK)
    {
        printf("accel enabled\n");
    }
    else
    {
        printf("ERROR\n");
        return;
    }

    lsm6dsox_accel_frame_t frame;
    while(1)
    {
        lsm6dsox_read_accel_data(&lsm6dsox, &frame);
        printf("x: %f\ny: %f\nz: %f\n", frame.x, frame.y, frame.z);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}