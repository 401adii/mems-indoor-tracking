#include "bno085.h"

#include <string.h>
#include <stdio.h>

#define BNO085_HEADER_MASK 0x7FFF
#define BNO085_HEADER_SHIFT 8

static void bno085_poll_hint(bno085_t *dev);
static void bno085_receive_frame(bno085_t *dev, uint8_t *buffer);

static void helper_print_buffer(uint8_t *buffer)
{
    for(uint16_t byte = 0; byte < 300; byte++)
    {
        printf("0x%2X | ", buffer[byte]);
    }
    printf("\n");
}

static void bno085_poll_hint(bno085_t *dev)
{
    while(dev->get_hint() != 0)
    {
        dev->delay(10);
    }
}


static void bno085_receive_frame(bno085_t *dev, uint8_t *buffer)
{
    memset((void *)buffer, 0, BNO085_BUFFER_SIZE);

    /*Get header*/
    dev->receive(buffer, BNO085_HEADER_SIZE);

    /*Extract size from header*/
    uint16_t size = (((uint16_t)buffer[BNO085_HEADER_LEN_MSB_BYTE] << BNO085_HEADER_SHIFT) | buffer[BNO085_HEADER_LEN_LSB_BYTE]) & BNO085_HEADER_MASK;
    printf("Size: %d\n", size);

    /*Receive an entire frame including header*/
    dev->receive(buffer, size);
}

bno085_status_t bno085_init(bno085_t *dev)
{
    if (!dev->receive  ||
        !dev->set_rst  ||
        !dev->get_hint ||
        !dev->delay    )
    {
        return BNO085_ERROR;
    }

    /*Module Reset Sequence*/
    dev->set_rst(1);
    dev->set_rst(0);
    dev->set_rst(1);

    uint8_t buffer[BNO085_BUFFER_SIZE] = {0};
    

    /*Waiting and reading all init frames*/
    while(1)
    {
        bno085_poll_hint(dev);
        bno085_receive_frame(dev, buffer);
        
        /*Break out when Reset Complete message transmitted on Executable Channel is received */
        if(buffer[BNO085_HEADER_CHANNEL_BYTE] == BNO085_CHANNEL_EXECUTABLE) 
        {
            break;
        }
    }
    
    return BNO085_OK;
}