#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum
{
    I2C_DEVICE_LSM6DSOX = 0,
    I2C_DEVICE_BNO085,
    I2C_DEVICE_NUM
} i2c_device_t;

void i2c_init_bus();
void i2c_add_device(i2c_device_t device, uint8_t address);

#endif // I2C_H_