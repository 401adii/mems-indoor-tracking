#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#define UART_BAUDRATE_DEFAULT 115200
#define UART_BUFFER_SIZE 1024
#define UART_QUEUE_SIZE 10

typedef void (*uart_callback_t)(uint8_t*, uint16_t);

void uart_init(uart_callback_t on_data_recv);

#endif // UART_H_