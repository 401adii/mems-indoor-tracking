#include "udp_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include <stdlib.h>

static uint16_t g_server_port;
static udp_callback_t g_user_callback = NULL;

static void udp_server_task(void *param)
{
    char rx_buffer[128];
    struct sockaddr_in dest_addr;

    while(1)
    {
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(g_server_port);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if(sock < 0)
        {
            break;
        }

        bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        struct sockaddr_storage source_addr;
        socklen_t socklen = sizeof(source_addr);

        while(1)
        {
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr*)&source_addr, &socklen);

            if(len > 0)
            {
                rx_buffer[len] = 0;
                if(g_user_callback != NULL)
                {
                    g_user_callback((uint8_t*)rx_buffer, len);
                }
                else
                {
                    break;
                }
            }
        }
        close(sock);
    }
    vTaskDelete(NULL);
}

void udp_server_start(uint16_t port, udp_callback_t on_data_recv)
{
    g_server_port = port;
    g_user_callback = on_data_recv;

    xTaskCreate(udp_server_task, "udp_server", 4096, NULL, 5, NULL);
}