#include "sensor_bus.h"
#include "i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

lsm6dsox_t g_lsm6dsox;
bno085_t g_bno085;

void lsm6dsox_transmit(uint8_t *data, uint16_t length)
{
    i2c_transmit(I2C_DEVICE_LSM6DSOX, data, length);
}

void lsm6dsox_receive(uint8_t *data, uint16_t length)
{
    i2c_receive(I2C_DEVICE_LSM6DSOX, data, length);
}

void bno085_transmit(uint8_t *data, uint16_t length)
{
    i2c_transmit(I2C_DEVICE_BNO085, data, length);
}

void bno085_receive(uint8_t *data, uint16_t length)
{
    i2c_receive(I2C_DEVICE_BNO085, data, length);
}

void bno085_set_rst(uint8_t level)
{
    gpio_set_level(BNO085_RESET_PIN, level);
}

uint8_t bno085_get_hint(void)
{
    return (uint8_t)gpio_get_level(BNO085_HINT_PIN);
}

void bno085_delay(uint32_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}