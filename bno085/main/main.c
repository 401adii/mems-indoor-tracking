#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "bno085.h"

#define MAX_DELAY pdMS_TO_TICKS(1000)

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

static bno085_t bno085 = {0};

static void bno085_receive(uint8_t *buffer, uint16_t size)
{
    i2c_master_receive(dev_handle, buffer, size, MAX_DELAY);
}

static void bno085_transmit(uint8_t* buffer, uint16_t size)
{
    i2c_master_transmit(dev_handle, buffer, size, MAX_DELAY);
}

static void bno085_set_rst(uint8_t level)
{
    gpio_set_level(BNO085_RESET_PIN, level);
}

static uint8_t bno085_get_hint()
{
    return (uint8_t)gpio_get_level(BNO085_HINT_PIN);
}

static void bno085_delay(uint32_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void app_main(void)
{   
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = 1,
    };
    i2c_new_master_bus(&bus_config, &bus_handle);

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = BNO085_I2C_ADDR,
        .scl_speed_hz = 100000,
    };
    i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);

    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << BNO085_RESET_PIN) | (1ULL << 2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&gpio_conf);
    gpio_conf.pin_bit_mask = (1ULL << BNO085_HINT_PIN);
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_conf);

    bno085.receive = bno085_receive;
    bno085.transmit = bno085_transmit;
    bno085.set_rst = bno085_set_rst;
    bno085.get_hint = bno085_get_hint;
    bno085.delay = bno085_delay;
    
    if(bno085_init(&bno085) == BNO085_OK)
    {
        ESP_LOGI("MAIN", "INIT OK");
    }
    else
    {
        ESP_LOGE("MAIN", "INIT ERROR");
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    bno085_enable_report(&bno085, BNO085_FEATURE_ID_LIN_ACCEL, 20000);
    uint8_t buffer[BNO085_BUFFER_SIZE] = {0};
    gpio_set_level(2, 1);
    while(1)
    {
        if(bno085_read_sensor_data(&bno085, BNO085_FEATURE_ID_LIN_ACCEL, buffer) == BNO085_OK)
        {
            printf("data\n");
        }
        else
        {
            printf("nodata\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
