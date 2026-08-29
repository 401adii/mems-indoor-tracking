#ifndef APP_TASKS_H_
#define APP_TASKS_H_

void led_blink_task(void* param);
void lsm6dsox_accel_task(void* param);
void bno085_accel_task(void* param);

#endif // APP_TASKS_H_