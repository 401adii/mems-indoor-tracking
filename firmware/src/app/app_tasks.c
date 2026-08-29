#include "app_tasks.h"
#include "sensor_bus.h" 
#include "gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define APP_TASKS_TAG "APP_TASKS"

void lsm6dsox_accel_task(void* param)
{
    ESP_LOGI(APP_TASKS_TAG, "LSM6DSOX task started");
    lsm6dsox_enable_accel(&g_lsm6dsox, LSM6DSOX_ACCEL_DEFAULT_CONFIG);
    lsm6dsox_accel_frame_t frame = {0};

    int64_t start_time = esp_timer_get_time();

    while((esp_timer_get_time() - start_time) < 5000000)
    {
        lsm6dsox_read_accel_data(&g_lsm6dsox, &frame);
        ESP_LOGI(APP_TASKS_TAG, "x: %.3f y: %.3f z: %.3f", frame.x, frame.y, frame.z);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

void bno085_accel_task(void* param)
{
    ESP_LOGI(APP_TASKS_TAG, "BNO085 task started");
    bno085_enable_report(&g_bno085, BNO085_FEATURE_ID_LIN_ACCEL, 20000);
    uint8_t buffer[BNO085_BUFFER_SIZE]= {0};
    bno085_lin_accel_frame_t frame = {0};

    int64_t start = esp_timer_get_time();

    while((esp_timer_get_time() - start) < 5000000)
    {
        if(bno085_read_sensor_data(&g_bno085, BNO085_FEATURE_ID_LIN_ACCEL, buffer) == BNO085_OK)
        {
            if(bno085_lin_accel_format_frame(buffer, &frame) == BNO085_OK)
            {
                ESP_LOGI(APP_TASKS_TAG, "x: %.3f y: %.3f z: %.3f", frame.x, frame.y, frame.z);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

void led_blink_task(void* param)
{
    gpio_set_level(GPIO_LED_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_LED_PIN, 0);
    vTaskDelete(NULL);
}