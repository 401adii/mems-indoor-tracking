#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "cmd_dispatcher.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define UART_PORT UART_NUM_0
#define BUFF_SIZE 1024
#define QUEUE_SIZE 10

QueueHandle_t queue;

cmd_ring_buffer_t rb = {0};

void led(void *param)
{
    gpio_set_level(2, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(2, 0);
    vTaskDelete(NULL);
}

void uart_rx(void* param)
{
    uart_config_t uart_config = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_DEFAULT,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_driver_install(UART_PORT, BUFF_SIZE, BUFF_SIZE, QUEUE_SIZE, &queue, 0);

    cmd_port_context_t uart_ctx = {
        .rx_buffer = &rb,
        .cmd_idx = 0,
    };
    
    uart_event_t event = {0};
    uint8_t data[BUFF_SIZE];

    while(1)
    {
        if(xQueueReceive(queue, &event, pdMS_TO_TICKS(1000)))
        {
            if(event.type == UART_DATA)
            {
                int bytes_read = uart_read_bytes(UART_PORT, data, event.size, pdMS_TO_TICKS(1000));
                cmd_buffer_push_frame(&rb, data, bytes_read);
                cmd_process_port(&uart_ctx);
            }
        }
    }
}

void app_main()
{   gpio_config_t gpio_conf = {
    .pin_bit_mask = (1 << 2),
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    
    gpio_config(&gpio_conf);
    xTaskCreatePinnedToCore(uart_rx, "uart", 4096, NULL, 0, NULL, 0);
    cmd_register_command(led, (uint8_t *)"led");
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}