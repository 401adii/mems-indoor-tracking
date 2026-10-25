#include "log_router.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LOG_ROUTER_BUFFER_SIZE 256

static log_output_target_t g_log_target = LOG_OUTPUT_SERIAL;
static int g_udp_sock = -1;
static struct sockaddr_in g_udp_dest_addr;
static bool g_udp_ready = false;
static portMUX_TYPE g_router_mux = portMUX_INITIALIZER_UNLOCKED;

static int log_vprintf(const char *fmt, va_list args)
{
    int ret = 0;

    if (g_log_target == LOG_OUTPUT_SERIAL) {
        ret = vprintf(fmt, args);
    } 
    else if (g_log_target == LOG_OUTPUT_UDP && g_udp_ready && g_udp_sock >= 0) {
        char log_buf[LOG_ROUTER_BUFFER_SIZE];
        ret = vsnprintf(log_buf, sizeof(log_buf), fmt, args);

        if (ret > 0) {
            struct sockaddr_in dest;
            portENTER_CRITICAL(&g_router_mux);
            dest = g_udp_dest_addr;
            portEXIT_CRITICAL(&g_router_mux);

            sendto(g_udp_sock, log_buf, ret, MSG_DONTWAIT, 
                   (struct sockaddr *)&dest, sizeof(dest));
        }
    }

    return ret;
}

void log_router_init(void)
{
    g_udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    esp_log_set_vprintf(log_vprintf);
}

void log_router_set_target(log_output_target_t target)
{
    g_log_target = target;
}

void log_router_set_udp_destination(const char *ip_str, uint16_t port)
{
    memset(&g_udp_dest_addr, 0, sizeof(g_udp_dest_addr));
    g_udp_dest_addr.sin_family = AF_INET;
    g_udp_dest_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip_str, &g_udp_dest_addr.sin_addr.s_addr);
    g_udp_ready = true;
}