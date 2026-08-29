#ifndef APP_CALLBACKS_H_
#define APP_CALLBACKS_H_

#include <stdint.h>

void handle_uart_data(uint8_t *data, uint16_t length);
void handle_udp_data(uint8_t *data, uint16_t length);

#endif // APP_CALLBACKS_H_