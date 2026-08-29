#include "udp_server.h"
#include "wifi_ap.h"
#include "cmd_dispatcher.h"
#include "i2c.h"
#include "gpio.h"
#include "bno085.h"
#include "lsm6dsox.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#include <stdint.h>
#include <stdlib.h>

#define MAIN_TAG "MAIN"

static lsm6dsox_t g_lsm6dsox;

static void lsm6dsox_transmit(uint8_t *data, uint16_t length);
static void lsm6dsox_receive(uint8_t *data, uint16_t length);

static bno085_t g_bno085;

static void bno085_transmit(uint8_t *data, uint16_t length);
static void bno085_receive(uint8_t *data, uint16_t length);
static void bno085_set_rst(uint8_t level);
static uint8_t bno085_get_hint();
static void bno085_delay(uint32_t delay_ms);

static cmd_ring_buffer_t g_uart_rb = {0};
static cmd_port_context_t g_uart_ctx = {
    .rx_buffer = &g_uart_rb,
    .cmd_idx = 0,
};

static cmd_ring_buffer_t g_udp_rb = {0};
static cmd_port_context_t g_udp_ctx = {
    .rx_buffer = &g_udp_rb,
    .cmd_idx = 0,
};

static void handle_uart_data(uint8_t *data, uint16_t length);
static void handle_udp_data(uint8_t *data, uint16_t length);

static void lsm6dsox_accel_task(void* param);
static void bno085_accel_task(void* param);

static const cmd_command_t g_command_list[] = {
    {lsm6dsox_accel_task, "LSM6DSOX_START"},
    {bno085_accel_task, "BNO085_START"}
};

void app_main(void)
{
    gpio_init();

    wifi_ap_init((const char *)"mems-indoor-tracker", (const char *)"12345678");
    udp_server_start(3333, handle_udp_data);
    
    i2c_status_t i2c_status = I2C_OK;
    
    i2c_status |= i2c_init_bus();
    i2c_status |= i2c_add_device(I2C_DEVICE_BNO085, BNO085_I2C_ADDR);
    i2c_status |= i2c_add_device(I2C_DEVICE_LSM6DSOX, LSM6DSOX_I2C_ADDR);
    
    if(i2c_status == I2C_ERROR)
    {
        ESP_LOGE(MAIN_TAG, "I2C_ERROR");
    }
    else
    {
        ESP_LOGI(MAIN_TAG, "I2C_OK");
    }

    lsm6dsox_status_t lsm6dsox_status = LSM6DSOX_OK;

    g_lsm6dsox.transmit = lsm6dsox_transmit;
    g_lsm6dsox.receive = lsm6dsox_receive;

    lsm6dsox_status = lsm6dsox_init(&g_lsm6dsox);
    
    if(lsm6dsox_status == LSM6DSOX_ERROR)
    {
        ESP_LOGE(MAIN_TAG, "LSM6DSOX_ERROR");
    }
    else
    {
        ESP_LOGI(MAIN_TAG, "LSM6DSOX_OK");
    }

    bno085_status_t bno085_status = BNO085_OK;

    g_bno085.transmit = bno085_transmit;
    g_bno085.receive = bno085_receive;
    g_bno085.set_rst = bno085_set_rst;
    g_bno085.get_hint = bno085_get_hint;
    g_bno085.delay = bno085_delay;

    bno085_status = bno085_init(&g_bno085);

    if(bno085_status == BNO085_ERROR)
    {
        ESP_LOGE(MAIN_TAG, "BNO085_ERROR");
    }
    else
    {
        ESP_LOGI(MAIN_TAG, "BNO085_OK");
    }

    size_t commands_num = sizeof(g_command_list) / sizeof(g_command_list[0]);
    
    for(size_t command = 0; command < commands_num; command++)
    {
        cmd_register_command(g_command_list[command].task, g_command_list[command].cmd_name);
    }
}

static void handle_uart_data(uint8_t *data, uint16_t length)
{
    if(length > 0)
    {
        cmd_buffer_push_frame(&g_uart_rb, data, length);
        cmd_process_port(&g_uart_ctx);
    }
}

static void handle_udp_data(uint8_t *data, uint16_t length)
{
    if(length > 0)
    {
        cmd_buffer_push_frame(&g_udp_rb, data, length);
        cmd_process_port(&g_udp_ctx);
    }
}

static void lsm6dsox_transmit(uint8_t *data, uint16_t length)
{
    i2c_transmit(I2C_DEVICE_LSM6DSOX, data, length);
}

static void lsm6dsox_receive(uint8_t *data, uint16_t length)
{
    i2c_receive(I2C_DEVICE_LSM6DSOX, data, length);
}

static void lsm6dsox_accel_task(void* param)
{
    ESP_LOGI(MAIN_TAG, "LSM6DSOX task started");
    lsm6dsox_enable_accel(&g_lsm6dsox, LSM6DSOX_ACCEL_DEFAULT_CONFIG);
    lsm6dsox_accel_frame_t frame = {0};

    int64_t start_time = esp_timer_get_time();

    while((esp_timer_get_time() - start_time) < 5000000)
    {
        lsm6dsox_read_accel_data(&g_lsm6dsox, &frame);
        ESP_LOGI(MAIN_TAG, "x: %.3f y: %.3f z: %.3f", frame.x, frame.y, frame.z);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

static void bno085_transmit(uint8_t *data, uint16_t length)
{
    i2c_transmit(I2C_DEVICE_BNO085, data, length);
}

static void bno085_receive(uint8_t *data, uint16_t length)
{
    i2c_receive(I2C_DEVICE_BNO085, data, length);
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

static void bno085_accel_task(void* param)
{
    ESP_LOGI(MAIN_TAG, "BNO085 task started");
    bno085_enable_report(&g_bno085, BNO085_FEATURE_ID_LIN_ACCEL, 20000);
    uint8_t buffer[BNO085_BUFFER_SIZE]= {0};
    bno085_lin_accel_frame_t frame = {0};

    int64_t start = esp_timer_get_time();

    while((esp_timer_get_time() - start) < 5000000)
    {
        if(bno085_read_sensor_data(&g_bno085, BNO085_FEATURE_ID_LIN_ACCEL, buffer) == BNO085_OK)
        {
            if(bno085_lin_accel_format_frame(buffer, &frame) == BNO085_OK)
            {
                ESP_LOGI(MAIN_TAG, "x: %.3f y: %.3f z: %.3f", frame.x, frame.y, frame.z);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}