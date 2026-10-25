#ifndef LOG_ROUTER_H_
#define LOG_ROUTER_H_

#include <stdint.h>

typedef enum
{
    LOG_OUTPUT_SERIAL = 0,
    LOG_OUTPUT_UDP
} log_output_target_t;

void log_router_init(void);
void log_router_set_target(log_output_target_t target);
void log_router_set_udp_destination(const char *ip_str, uint16_t port);

#endif // LOG_ROUTER_H_