#include "udp_server.h"
#include "wifi_ap.h"
#include "i2c.h"
#include "bno085.h"
#include "lsm6dsox.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define MAIN_TAG "MAIN"

#include <stdio.h>

static bno085_t g_bno085;
static lsm6dsox_t g_lsm6dsox;

static void handle_udp_data(uint8_t *data, uint16_t length);
static void lsm6dsox_transmit(uint8_t *data, uint16_t length);
static void lsm6dsox_receive(uint8_t *data, uint16_t length);

void app_main(void)
{
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