#include <stdio.h>
#include "bno085.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BNO085_I2C_ADDR 0x4A /*0x4B*/
#define BNO085_RESET_PIN GPIO_NUM_23
#define BNO085_HINT_PIN GPIO_NUM_19

static bno085_t bno085 = {0};
static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

static void delay(uint32_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

static void reset()
{
    gpio_set_level(BNO085_RESET_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BNO085_RESET_PIN, 1);
}

static void transmit(uint8_t* data, uint16_t len)
{
    i2c_master_transmit(dev_handle, data, len, 1000);
}

static void receive(uint8_t *data, uint16_t len)
{
    i2c_master_receive(dev_handle, data, len, 1000);
}

static void bno085_read_packet(uint8_t *rx_buffer, size_t max_len)
{
    uint8_t header[4];

    bno085.receive(header, 4);
    uint16_t total_len = ((header[1] << 8) | header[0]) & 0x7FFF;

    if(total_len < 4)
    {
        return;
    }

    for(int i = 0; i < 4; i++)
    {
        rx_buffer[i] = header[i];
    }

    if(total_len == 4)
    {
        return;
    }

    size_t payload_len = total_len - 4;
    if(total_len > max_len)
    {
        payload_len = max_len - 4;
    }

    bno085.receive(&rx_buffer[4], payload_len);
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

    bno085.delay = delay;
    bno085.reset = reset;
    bno085.transmit = transmit;
    bno085.receive = receive;

    bno085.reset();

    while(gpio_get_level(BNO085_HINT_PIN) != 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    uint8_t tx_buffer[6] = {
        0x06, 0x00,
        0x02,      
        0x00,      
        0xF9,      
        0x00       
    };

    ESP_LOGI("MAIN", "Sending Product ID Request (0xF9)...");
    bno085.transmit(tx_buffer, sizeof(tx_buffer));

    while(gpio_get_level(BNO085_HINT_PIN) != 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI("MAIN", "HINT PIN LOW - Response Ready!");

    uint8_t rx_buffer[512] = {0}; 
    bno085_read_packet(rx_buffer, sizeof(rx_buffer));

    uint16_t resp_len  = ((rx_buffer[1] << 8) | rx_buffer[0]) & 0x7FFF;
    uint8_t  channel   = rx_buffer[2];
    uint8_t  report_id = rx_buffer[4];

    if (channel == 2 && report_id == 0xF8)
    {
        uint8_t reset_cause = rx_buffer[5];
        uint8_t sw_major    = rx_buffer[6];
        uint8_t sw_minor    = rx_buffer[7];
        
        /* Part number and build number are 32-bit little-endian integers */
        uint32_t part_num  = (rx_buffer[11] << 24) | (rx_buffer[10] << 16) | (rx_buffer[9] << 8)  | rx_buffer[8];
        uint32_t build_num = (rx_buffer[15] << 24) | (rx_buffer[14] << 16) | (rx_buffer[13] << 8) | rx_buffer[12];
        uint16_t sw_patch  = (rx_buffer[17] << 8)  | rx_buffer[16];

        ESP_LOGI("MAIN", "=== BNO085 Two-Way Comms Verified! ===");
        ESP_LOGI("MAIN", "Part Number: %lu", part_num);
        ESP_LOGI("MAIN", "Firmware:    v%d.%d.%d (Build %lu)", sw_major, sw_minor, sw_patch, build_num);
        ESP_LOGI("MAIN", "Reset Cause: 0x%02X", reset_cause);
    }
    else
    {
        ESP_LOGE("MAIN", "Unexpected packet! Channel: %d | Report ID: 0x%02X", channel, report_id);
        ESP_LOG_BUFFER_HEX("MAIN", rx_buffer, resp_len);
    }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
