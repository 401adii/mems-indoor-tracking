#ifndef LSM6DSOX_H_
#define LSM6DSOX_H_

#include <stdint.h>

#define LSM6DSOX_I2C_ADDRESS 0x6A /*0x6B*/

#define LSM6DSOX_G 9.80665f

#define LSM6DSOX_REG_WHOAMI 0x0F
#define LSM6DSOX_REG_CTRL1_XL 0x10
#define LSM6DSOX_REG_OUTX_L_A 0x28

#define LSM6DSOX_WHOAMI_VAL 0x6C

#define LSM6DSOX_ACCEL_DEFAULT_CONFIG 0x47

typedef void (*bno085_transmit_func_t)(uint8_t*, uint16_t);
typedef void (*bno085_receive_func_t)(uint8_t*, uint16_t);

typedef enum
{
    LSM6DSOX_OK = 0,
    LSM6DSOX_ERROR,
} lsm6dsox_status_t;

typedef struct
{
    bno085_transmit_func_t transmit;
    bno085_receive_func_t receive;
} lsm6dsox_t;

typedef struct
{
    float x;
    float y;
    float z;
} lsm6dsox_accel_frame_t;

lsm6dsox_status_t lsm6dsox_init(lsm6dsox_t *dev);
lsm6dsox_status_t lsm6dsox_enable_accel(lsm6dsox_t *dev, uint8_t config_byte);
void lsm6dsox_read_accel_data(lsm6dsox_t *dev, lsm6dsox_accel_frame_t *frame);

#endif /* LSM6DSOX_H_ */