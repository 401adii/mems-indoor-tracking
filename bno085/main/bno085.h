#ifndef BNO085_H_
#define BNO085_H_

#include <stdint.h>

#define BNO085_I2C_ADDR 0x4A
#define BNO085_RESET_PIN 23
#define BNO085_HINT_PIN 19

#define BNO085_HEADER_SIZE 4

#define BNO085_BUFFER_SIZE 1024

typedef enum
{
    BNO085_OK = 0,
    BNO085_ERROR
} bno085_status_t;

typedef void(*bno085_receive_func_t)(uint8_t*);
typedef void(*bno085_set_rst_func_t)(uint8_t);
typedef uint8_t(*bno085_get_hint_func_t)(void);
typedef void(*bno085_delay_func_t)(uint32_t);

typedef struct 
{
    bno085_receive_func_t receive;
    bno085_set_rst_func_t set_rst;
    bno085_get_hint_func_t get_hint;
    bno085_delay_func_t delay;
} bno085_t;

bno085_status_t bno085_init(bno085_t *dev);

#endif /*BNO085_H_*/