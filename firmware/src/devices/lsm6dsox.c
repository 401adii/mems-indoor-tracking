#include "lsm6dsox.h"

lsm6dsox_status_t lsm6dsox_init(lsm6dsox_t *dev)
{
    if (!dev->transmit ||
        !dev->receive)
    {
        return LSM6DSOX_ERROR;
    }

    uint8_t byte = LSM6DSOX_REG_WHOAMI;
    dev->transmit(&byte, 1);
    dev->receive(&byte, 1);
    
    if(byte != LSM6DSOX_WHOAMI_VAL)
    {
        return LSM6DSOX_ERROR;
    }
    return LSM6DSOX_OK;
}

lsm6dsox_status_t lsm6dsox_enable_accel(lsm6dsox_t *dev, uint8_t config_byte)
{
    uint8_t buffer[] = {LSM6DSOX_REG_CTRL1_XL, config_byte};
    // Send configuration data
    dev->transmit(buffer, sizeof(buffer));
    // Verify if data got written correctly
    dev->transmit(&buffer[0], 1);
    dev->receive(&buffer[0], 1);

    if(buffer[0] != config_byte)
    {
        return LSM6DSOX_ERROR;
    }
    return LSM6DSOX_OK;
}   

void lsm6dsox_read_accel_data(lsm6dsox_t *dev, lsm6dsox_accel_frame_t *frame)
{
    uint8_t buffer[6] = {0};
    buffer[0] = LSM6DSOX_REG_OUTX_L_A;
    dev->transmit(&buffer[0], 1);
    dev->receive(buffer, 6);

    int16_t x_raw = (buffer[1] << 8) | buffer[0];
    int16_t y_raw = (buffer[3] << 8) | buffer[2];
    int16_t z_raw = (buffer[5] << 8) | buffer[4];
    // todo: Automate the 0.061 value depending on config.
    // m/s^2
    frame->x = x_raw * 0.061 / 1000 * LSM6DSOX_G;
    frame->y = y_raw * 0.061 / 1000 * LSM6DSOX_G;
    frame->z = z_raw * 0.061 / 1000 * LSM6DSOX_G;
}