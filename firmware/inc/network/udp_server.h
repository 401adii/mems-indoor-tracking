#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include <stdint.h>

#define UDP_DEFAULT_TRANSMIT_PORT 4554

typedef void (*udp_callback_t)(uint8_t*, uint16_t);

void udp_server_start(uint16_t port, udp_callback_t on_data_recv);

#endif // UDP_SERVER_H_