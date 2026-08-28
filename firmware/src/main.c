#include "udp_server.h"
#include "wifi_ap.h"
#include "i2c.h"
#include "bno085.h"
#include "lsm6dsox.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

void handle_udp_data(uint8_t *data, uint16_t length)
{
    printf("%s\n", data);
}

void app_main(void)
{
    wifi_ap_init((const char *)"mems-indoor-tracker", (const char *)"12345678");
    udp_server_start(3333, handle_udp_data);
    i2c_init_bus();
    i2c_add_device(I2C_DEVICE_BNO085, BNO085_I2C_ADDR);
    i2c_add_device(I2C_DEVICE_LSM6DSOX, LSM6DSOX_I2C_ADDR);
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
