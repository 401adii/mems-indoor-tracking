#include "bno085.h"

bno085_status_t bno085_init(bno085_t *dev)
{
    if (!dev->receive ||
        !dev->set_rst)
    {
        return BNO085_ERROR;
    }
    return BNO085_OK;
}