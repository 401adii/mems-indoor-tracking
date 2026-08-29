#include "app_callbacks.h"
#include "cmd_dispatcher.h"

static cmd_ring_buffer_t g_uart_rb = {0};
static cmd_port_context_t g_uart_ctx = {
    .rx_buffer = &g_uart_rb,
    .cmd_idx = 0,
};

static cmd_ring_buffer_t g_udp_rb = {0};
static cmd_port_context_t g_udp_ctx = {
    .rx_buffer = &g_udp_rb,
    .cmd_idx = 0,
};

void handle_uart_data(uint8_t *data, uint16_t length)
{
    if(length > 0)
    {
        cmd_buffer_push_frame(&g_uart_rb, data, length);
        cmd_process_port(&g_uart_ctx);
    }
}

void handle_udp_data(uint8_t *data, uint16_t length)
{
    if(length > 0)
    {
        cmd_buffer_push_frame(&g_udp_rb, data, length);
        cmd_process_port(&g_udp_ctx);
    }
}