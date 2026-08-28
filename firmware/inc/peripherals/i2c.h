#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum
{
    I2C_OK = 0,
    I2C_ERROR
} i2c_status_t;

typedef enum
{
    I2C_DEVICE_LSM6DSOX = 0,
    I2C_DEVICE_BNO085,
    I2C_DEVICE_NUM
} i2c_device_t;

i2c_status_t i2c_init_bus();
i2c_status_t i2c_add_device(i2c_device_t device, uint8_t address);
i2c_status_t i2c_transmit(i2c_device_t device, uint8_t* data, uint16_t length);
i2c_status_t i2c_receive(i2c_device_t device, uint8_t* data, uint16_t length);

#endif // I2C_H_