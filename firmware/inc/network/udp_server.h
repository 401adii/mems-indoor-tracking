#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include <stdint.h>

typedef void (*udp_callback_t)(uint8_t *data, uint16_t length);

void udp_server_start(uint16_t port, udp_callback_t on_data_recv);

#endif // UDP_SERVER_H_