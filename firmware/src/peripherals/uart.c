#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <stdlib.h>

#define UART_DEFAULT_TIMEOUT pdMS_TO_TICKS(1000)

static uart_callback_t g_user_callback;
QueueHandle_t g_queue;

static void uart_rx_task(void* param)
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUDRATE_DEFAULT,
        .data_bits = UART_DATA_8_BITS,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_DEFAULT,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, UART_BUFFER_SIZE, UART_BUFFER_SIZE, UART_QUEUE_SIZE, &g_queue, 0);

    uart_event_t event = {0};
    uint8_t data[UART_BUFFER_SIZE];

    while(1)
    {
        if(xQueueReceive(g_queue, &event, UART_DEFAULT_TIMEOUT))
        {
            if(event.type == UART_DATA)
            {
                uint16_t bytes_read = uart_read_bytes(UART_NUM_0, data, event.size, UART_DEFAULT_TIMEOUT);
                g_user_callback(data, bytes_read);
            }
        }
    }
}

void uart_init(uart_callback_t on_data_recv)
{
    g_user_callback = on_data_recv;

    xTaskCreate(uart_rx_task, "uart_rx", 4096, NULL, 5, NULL);
}