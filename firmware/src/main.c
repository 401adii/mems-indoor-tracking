#include "udp_server.h"
#include "wifi_ap.h"
#include "i2c.h"
#include "gpio.h"
#include "bno085.h"
#include "lsm6dsox.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include <stdint.h>

#define MAIN_TAG "MAIN"

static bno085_t g_bno085;
static lsm6dsox_t g_lsm6dsox;

static void handle_udp_data(uint8_t *data, uint16_t length);
static void lsm6dsox_transmit(uint8_t *data, uint16_t length);
static void lsm6dsox_receive(uint8_t *data, uint16_t length);
static void bno085_transmit(uint8_t *data, uint16_t length);
static void bno085_receive(uint8_t *data, uint16_t length);
static void bno085_set_rst(uint8_t level);
static uint8_t bno085_get_hint();
static void bno085_delay(uint32_t delay_ms);


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
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void handle_udp_data(uint8_t *data, uint16_t length)
{
    printf("%s\n", data);
}

static void lsm6dsox_transmit(uint8_t *data, uint16_t length)
{
    i2c_transmit(I2C_DEVICE_LSM6DSOX, data, length);
}

static void lsm6dsox_receive(uint8_t *data, uint16_t length)
{
    i2c_receive(I2C_DEVICE_LSM6DSOX, data, length);
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