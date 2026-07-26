#include "cmd_dispatcher.h"
#include <string.h>

static cmd_command_t g_cmd_table[CMD_COMMAND_NUM] = {0};
static uint16_t g_cmd_num = 0;
static cmd_args_t g_cmd_args = {0};

void cmd_register_command(TaskFunction_t task, uint8_t *cmd_name)
{
    if(task == NULL || cmd_name == NULL)
    {
        return;
    }

    if(g_cmd_num >= CMD_COMMAND_NUM - 1)
    {
        return;
    }

    for (uint16_t i = 0; i < g_cmd_num; i++)
    {
        if(strcmp((const char *)cmd_name,(const char *) g_cmd_table[i].cmd_name) == 0)
        {
            return;
        }
    }

    g_cmd_table[g_cmd_num].task = task;
    memcpy(g_cmd_table[g_cmd_num].cmd_name, cmd_name, strlen((const char *)cmd_name));
    g_cmd_num++;
}

void cmd_buffer_push_byte(cmd_ring_buffer_t* rb, uint8_t data)
{
    uint8_t next_head = (rb->head + 1) % CMD_BUFFER_SIZE;
    if(next_head != rb->tail)
    {
        rb->buffer[rb->head] = data;
        rb->head = next_head;
    }
}

void cmd_buffer_push_frame(cmd_ring_buffer_t* rb, uint8_t* data, uint16_t len)
{
    for(uint16_t byte = 0; byte < len; byte++)
    {
        cmd_buffer_push_byte(rb, data[byte]);
    }
}

uint8_t cmd_buffer_pop_byte(cmd_ring_buffer_t* rb, uint8_t* data)
{
    if (rb->head == rb->tail)
    {
        return 0;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % CMD_BUFFER_SIZE;
    return 1;
}

void cmd_process_port(cmd_port_context_t *ctx)
{
    uint8_t byte;

    while(cmd_buffer_pop_byte(ctx->rx_buffer, &byte))
    {
        if(byte == '\n')
        {
            if(ctx->cmd_idx > 0)
            {
                ctx->cmd_buffer[ctx->cmd_idx] = '\0';
                cmd_dispatch_command(ctx->cmd_buffer);
                ctx->cmd_idx = 0;
            }
        }
        else
        {
            if(ctx->cmd_idx < CMD_BUFFER_SIZE - 1)
            {
                ctx->cmd_buffer[ctx->cmd_idx] = byte;
                ctx->cmd_idx++;
            }
            else
            {
                ctx->cmd_idx = 0;
            }
        }
    }
}

void cmd_dispatch_command(uint8_t* cmd_string)
{
    uint8_t* cmd_name = (uint8_t *)strtok((char *)cmd_string, " ");

    if(cmd_name == NULL)
    {
        return;
    }

    memset(&g_cmd_args, 0, sizeof(cmd_args_t));

    char* token = strtok(NULL, " ");
    while(token != NULL && g_cmd_args.argc < CMD_MAX_ARGS)
    {
        strncpy(g_cmd_args.argv[g_cmd_args.argc], token, CMD_ARG_LEN - 1);
        g_cmd_args.argv[g_cmd_args.argc][CMD_ARG_LEN - 1] = '\0';
        g_cmd_args.argc++;
        token = strtok(NULL, " ");
    }

    for(uint16_t i = 0; i < CMD_COMMAND_NUM; i++)
    {
        if(strcmp((const char*)cmd_name, (const char*)g_cmd_table[i].cmd_name) == 0)
        {
            xTaskCreatePinnedToCore(
                g_cmd_table[i].task,
                (const char *)g_cmd_table[i].cmd_name, 
                CMD_TASK_STACK_DEPTH, 
                (void*) &g_cmd_args, 
                CMD_TASK_PRIORITY, 
                NULL,
                0);
        }
    }
}