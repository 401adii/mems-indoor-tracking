#ifndef CMD_DISPATCHER_H_
#define CMD_DISPATCHER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#define CMD_BUFFER_SIZE 128
#define CMD_COMMAND_NUM 128

#define CMD_MAX_ARGS 6
#define CMD_ARG_LEN 16

#define CMD_TASK_STACK_DEPTH 4096
#define CMD_TASK_PRIORITY 2

typedef struct
{
    uint8_t buffer[CMD_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} cmd_ring_buffer_t;

typedef struct
{
    cmd_ring_buffer_t* rx_buffer;
    uint8_t cmd_buffer[CMD_BUFFER_SIZE];
    uint16_t cmd_idx;
} cmd_port_context_t;

typedef struct
{
    TaskFunction_t task;
    uint8_t cmd_name[CMD_BUFFER_SIZE];
} cmd_command_t;

typedef struct
{
    int argc;
    char argv[CMD_MAX_ARGS][CMD_ARG_LEN];
} cmd_args_t;

void cmd_register_command(TaskFunction_t task, uint8_t* cmd_name);
void cmd_buffer_push_byte(cmd_ring_buffer_t* rb, uint8_t data);
void cmd_buffer_push_frame(cmd_ring_buffer_t* rb, uint8_t* data, uint16_t len);
uint8_t cmd_buffer_pop_byte(cmd_ring_buffer_t* rb, uint8_t* data);
void cmd_process_port(cmd_port_context_t* ctx);
void cmd_dispatch_command(uint8_t* cmd_string);

#endif // CMD_DISPATCHER_H_