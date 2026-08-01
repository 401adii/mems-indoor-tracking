#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BNO085_I2C_ADDR 0x4A /*0x4B*/
#define BNO085_RESET_PIN GPIO_NUM_23
#define BNO085_HINT_PIN GPIO_NUM_19

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

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
        .pin_bit_mask = (1ULL << BNO085_RESET_PIN),
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

    gpio_set_level(BNO085_RESET_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BNO085_RESET_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BNO085_RESET_PIN, 1);

    uint8_t hint_level = (uint8_t)gpio_get_level(BNO085_HINT_PIN);
    uint8_t buffer[1024] = {0};

    while((uint8_t)gpio_get_level(BNO085_HINT_PIN) == hint_level)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    ESP_LOGI("MAIN", "HINT READY!");
    
    i2c_master_receive(dev_handle, buffer, 4, pdMS_TO_TICKS(1000));
    ESP_LOGI("MAIN", "Byte0: %d | Byte1: %d | Byte2: %d | Byte3: %d", buffer[0], buffer[1], buffer[2], buffer[3]);
    
    uint16_t size = (((uint16_t)buffer[1] << 8) | buffer[0]) & 0x7FFF;
    ESP_LOGI("MAIN", "Size: %d", size);
    memset((void*)buffer, 0, 1024);
    
    i2c_master_receive(dev_handle, buffer, size, pdMS_TO_TICKS(1000));
    memset((void*)buffer, 0, 1024);

    hint_level = gpio_get_level(BNO085_HINT_PIN);
    while((uint8_t)gpio_get_level(BNO085_HINT_PIN) == hint_level)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    i2c_master_receive(dev_handle, buffer, 4, pdMS_TO_TICKS(1000));
    size = (((uint16_t)buffer[1] << 8) | buffer[0]) & 0x7FFF;
    ESP_LOGI("MAIN", "Byte0: %d | Byte1: %d | Byte2: %d | Byte3: %d", buffer[0], buffer[1], buffer[2], buffer[3]);
    memset((void*)buffer, 0, 1024);
    i2c_master_receive(dev_handle, buffer, size, pdMS_TO_TICKS(1000));
    for(uint16_t byte = 0; byte < size; byte++)
    {
        ESP_LOGI("MAIN", "Byte%d: %d", byte, buffer[byte]);
    }
    memset((void*)buffer, 0, 1024);

    i2c_master_receive(dev_handle, buffer, 4, pdMS_TO_TICKS(1000));
    size = (((uint16_t)buffer[1] << 8) | buffer[0]) & 0x7FFF;
    ESP_LOGI("MAIN", "Byte0: %d | Byte1: %d | Byte2: %d | Byte3: %d", buffer[0], buffer[1], buffer[2], buffer[3]);
    memset((void*)buffer, 0, 1024);
    i2c_master_receive(dev_handle, buffer, size, pdMS_TO_TICKS(1000));
    for(uint16_t byte = 0; byte < size; byte++)
    {
        ESP_LOGI("MAIN", "Byte%d: %d", byte, buffer[byte]);
    }
    memset((void*)buffer, 0, 1024);
    
    buffer[0] = 21;
    buffer[1] = 0;
    buffer[2] = 2;
    buffer[3] = 0;
    buffer[4] = 253;
    buffer[5] = 6;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0;
    buffer[9] = 32;
    buffer[10] = 78;
    buffer[11] = 0;
    buffer[12] = 0;
    buffer[13] = 0;
    buffer[14] = 0;
    buffer[15] = 0;
    buffer[16] = 0;
    buffer[17] = 0;
    buffer[18] = 0;
    buffer[19] = 0;
    buffer[20] = 0;

    ESP_LOGI("MAIN", "TRANSMITING DATA");
    i2c_master_transmit(dev_handle, buffer, 21, pdMS_TO_TICKS(1000));

    hint_level = (uint8_t)gpio_get_level(BNO085_HINT_PIN);
    while((uint8_t)gpio_get_level(BNO085_HINT_PIN) == hint_level)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI("MAIN", "ACK READY");
    i2c_master_receive(dev_handle, buffer, 4, pdMS_TO_TICKS(1000));
    ESP_LOGI("MAIN", "Byte0: %d | Byte1: %d | Byte2: %d | Byte3: %d", buffer[0], buffer[1], buffer[2], buffer[3]);

    size = (((uint16_t)buffer[1] << 8) | buffer[0]) & 0x7FFF;
    ESP_LOGI("MAIN", "ACK Frame Size: %d", size);
    for(uint16_t byte = 0; byte < size; byte++)
    {
        ESP_LOGI("MAIN", "Byte%d: %d", byte, buffer[byte]);
    }
    memset((void*)buffer, 0 , 1024);
    
    i2c_master_receive(dev_handle, buffer, size, pdMS_TO_TICKS(1000));
    
    memset((void*)buffer, 0, 1024);
    hint_level = (uint8_t)gpio_get_level(BNO085_HINT_PIN);
    ESP_LOGI("MAIN", "HINT LEVEL: %d", hint_level);
    while(1)
    {
        while((uint8_t)gpio_get_level(BNO085_HINT_PIN) == hint_level)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        
        ESP_LOGI("MAIN", "DATA READY");
        i2c_master_receive(dev_handle, buffer, 4, pdMS_TO_TICKS(1000));
        ESP_LOGI("MAIN", "Byte0: %d | Byte1: %d | Byte2: %d | Byte3: %d", buffer[0], buffer[1], buffer[2], buffer[3]);

        size = (((uint16_t)buffer[1] << 8) | buffer[0]) & 0x7FFF;
        ESP_LOGI("MAIN", "Data Frame Size: %d", size);
        memset((void*)buffer, 0 , 1024);
        
        i2c_master_receive(dev_handle, buffer, size, pdMS_TO_TICKS(1000));
        for(uint16_t byte = 0; byte < size; byte++)
        {
            ESP_LOGI("MAIN", "Byte%d: %d", byte, buffer[byte]);
        }
        memset((void*)buffer, 0, 1024);
    }
}
