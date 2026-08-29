#ifndef SENSOR_BUS_H_
#define SENSOR_BUS_H_

#include "lsm6dsox.h"
#include "bno085.h"
#include <stdint.h>

extern lsm6dsox_t g_lsm6dsox;
extern bno085_t g_bno085;

void lsm6dsox_transmit(uint8_t *data, uint16_t length);
void lsm6dsox_receive(uint8_t *data, uint16_t length);

void bno085_transmit(uint8_t *data, uint16_t length);
void bno085_receive(uint8_t *data, uint16_t length);
void bno085_set_rst(uint8_t level);
uint8_t bno085_get_hint(void);
void bno085_delay(uint32_t delay_ms);

#endif // SENSOR_BUS_H_