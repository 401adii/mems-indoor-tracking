#include "bno085.h"

static void bno085_poll_hint(bno085_t *dev);

static void bno085_poll_hint(bno085_t *dev)
{
    while(dev->get_hint() != 0)
    {
        dev->delay(10);
    }
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

    /*Waiting for init frame*/
    bno085_poll_hint(dev);

    /*Reading init frame*/
    dev->receive(buffer);

    return BNO085_OK;
}