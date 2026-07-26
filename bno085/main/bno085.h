#ifndef BNO085_H_
#define BNO085_H_

typedef void(*bno085_transmit_func_t)(uint8_t*, uint16_t);
typedef void(*bno085_receive_func_t)(uint8_t*, uint16_t);
typedef void(*bno085_reset_func_t)();
typedef void(*bno085_handle_hint_func_t)();
typedef void(*bno085_delay_func_t)(uint32_t);
typedef void(*bno085_get_system_time_func_t)();

typedef struct
{
    bno085_transmit_func_t transmit;
    bno085_receive_func_t receive;
    bno085_reset_func_t reset;
    bno085_handle_hint_func_t handle_hint;
    bno085_delay_func_t delay;
    bno085_get_system_time_func_t get_time;
} bno085_t;


#endif /*BNO085_H_*/