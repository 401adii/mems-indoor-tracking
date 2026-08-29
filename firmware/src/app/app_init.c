#include "app_init.h"
#include "app_callbacks.h"
#include "app_tasks.h"
#include "sensor_bus.h" 
#include "cmd_dispatcher.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "wifi_ap.h"
#include "udp_server.h"
#include "esp_log.h"
#include <stddef.h>

#define APP_INIT_TAG "APP_INIT"

static cmd_command_t g_command_list[] = {
    {led_blink_task, "BLINK"},
    {lsm6dsox_accel_task, "LSM6DSOX_START"},
    {bno085_accel_task, "BNO085_START"}
};

void app_init(void)
{
    gpio_init();
    
    uart_init(handle_uart_data);

    wifi_ap_init("mems-indoor-tracker", "12345678");
    udp_server_start(3333, handle_udp_data);
    
    i2c_status_t i2c_status = I2C_OK;
    i2c_status |= i2c_init_bus();
    i2c_status |= i2c_add_device(I2C_DEVICE_BNO085, BNO085_I2C_ADDR);
    i2c_status |= i2c_add_device(I2C_DEVICE_LSM6DSOX, LSM6DSOX_I2C_ADDR);
    
    if(i2c_status == I2C_ERROR)
    {
        ESP_LOGE(APP_INIT_TAG, "I2C_ERROR");
    }
    else
    {
        ESP_LOGI(APP_INIT_TAG, "I2C_OK");
    }

    g_lsm6dsox.transmit = lsm6dsox_transmit;
    g_lsm6dsox.receive = lsm6dsox_receive;

    if(lsm6dsox_init(&g_lsm6dsox) == LSM6DSOX_ERROR)
    {
        ESP_LOGE(APP_INIT_TAG, "LSM6DSOX_ERROR");
    }
    else
    {
        ESP_LOGI(APP_INIT_TAG, "LSM6DSOX_OK");
    }

    g_bno085.transmit = bno085_transmit;
    g_bno085.receive = bno085_receive;
    g_bno085.set_rst = bno085_set_rst;
    g_bno085.get_hint = bno085_get_hint;
    g_bno085.delay = bno085_delay;

    if(bno085_init(&g_bno085) == BNO085_ERROR)
    {
        ESP_LOGE(APP_INIT_TAG, "BNO085_ERROR");
    }
    else
    {
        ESP_LOGI(APP_INIT_TAG, "BNO085_OK");
    }

    size_t commands_num = sizeof(g_command_list) / sizeof(g_command_list[0]);

    for(size_t command = 0; command < commands_num; command++)
    {
        cmd_register_command(g_command_list[command].task, g_command_list[command].cmd_name);
    }
}