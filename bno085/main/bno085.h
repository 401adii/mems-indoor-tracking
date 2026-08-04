#ifndef BNO085_H_
#define BNO085_H_

#include <stdint.h>

#define BNO085_I2C_ADDR     0x4A
#define BNO085_RESET_PIN    23
#define BNO085_HINT_PIN     19

#define BNO085_CHANNEL_COMMAND          0 
#define BNO085_CHANNEL_EXECUTABLE       1
#define BNO085_CHANNEL_SENSOR_CONTROL   2
#define BNO085_CHANNEL_INPUT_REPORTS    3

#define BNO085_HEADER_SIZE          4
#define BNO085_HEADER_LEN_LSB_BYTE  0
#define BNO085_HEADER_LEN_MSB_BYTE  1
#define BNO085_HEADER_CHANNEL_BYTE  2
#define BNO085_HEADER_SEQNUM_BYTE   3

#define BNO085_COMMAND_SET_FEATURE      0xFD
#define BNO085_COMMAND_SET_FEATURE_LEN  21
#define BNO085_FEATURE_ID_LIN_ACCEL     4

#define BNO085_BUFFER_SIZE 1024

typedef enum
{
    BNO085_OK = 0,
    BNO085_ERROR
} bno085_status_t;

typedef void(*bno085_receive_func_t)(uint8_t*, uint16_t);
typedef void(*bno085_transmit_func_t)(uint8_t*, uint16_t);
typedef void(*bno085_set_rst_func_t)(uint8_t);
typedef uint8_t(*bno085_get_hint_func_t)(void);
typedef void(*bno085_delay_func_t)(uint32_t);

typedef struct 
{
    bno085_receive_func_t receive;
    bno085_transmit_func_t transmit;
    bno085_set_rst_func_t set_rst;
    bno085_get_hint_func_t get_hint;
    bno085_delay_func_t delay;
} bno085_t;

bno085_status_t bno085_init(bno085_t *dev);
void bno085_enable_report(bno085_t *dev, uint8_t feature_id, uint32_t report_interval_hz);
bno085_status_t bno085_read_sensor_data(bno085_t *dev, uint8_t feature_id, uint8_t *buffer);

#endif /*BNO085_H_*/