#include "app_tasks.h"
#include "sensor_bus.h" 
#include "cmd_dispatcher.h"
#include "log_router.h"
#include "udp_server.h"
#include "gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#include <stdbool.h>
#include <string.h>

#define APP_TASKS_TAG "APP_TASKS"
#define APP_TASKS_SELECTED_PRIORITY 5

static TaskFunction_t g_selected_task = NULL;
static TaskHandle_t g_selected_task_handle = NULL;
static volatile bool g_task_stop_requested = false;

static cmd_command_t g_tasks_table[] = {
    {led_blink_task, "BLINK"},
    {lsm6dsox_accel_task, "LSM6DSOX"},
    {bno085_accel_task, "BNO085"}
};

void lsm6dsox_accel_task(void* param)
{
    ESP_LOGI(APP_TASKS_TAG, "LSM6DSOX task started");
    lsm6dsox_enable_accel(&g_lsm6dsox, LSM6DSOX_ACCEL_DEFAULT_CONFIG);
    lsm6dsox_accel_frame_t frame = {0};

    while(!g_task_stop_requested)
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

    while(!g_task_stop_requested)
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
    while(!g_task_stop_requested)
    {
        gpio_set_level(GPIO_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(GPIO_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    gpio_set_level(GPIO_LED_PIN, 0);
    
    g_selected_task_handle = NULL;
    vTaskDelete(NULL);
}

void select_task(void* param)
{
    cmd_args_t *args = (cmd_args_t *)param;
    
    if(args->argc != 1)
    {
        ESP_LOGW(APP_TASKS_TAG, "Expected 1 argument, got %d. Aborting selection.", args->argc);
        vTaskDelete(NULL);
        return;
    }

    uint16_t task_table_len = sizeof(g_tasks_table) / sizeof(g_tasks_table[0]);
    bool found = false;
    for(uint16_t task = 0; task < task_table_len; task++)
    {
        if(strcmp((const char *)g_tasks_table[task].cmd_name, (const char *)args->argv[0]) == 0)
        {
            g_selected_task = g_tasks_table[task].task;
            found = true;
            ESP_LOGI(APP_TASKS_TAG, "Successfully selected task %s", args->argv[0]);
            break;
        }
    }

    if(!found)
    {
        ESP_LOGW(APP_TASKS_TAG, "Failed to find task %s", args->argv[0]);
    }
    
    vTaskDelete(NULL);
}

void start_task(void *param)
{
    if(g_selected_task == NULL || g_selected_task_handle != NULL)
    {
        ESP_LOGW(APP_TASKS_TAG, "No task is selected. Aborting start.");
        vTaskDelete(NULL);
        return;
    }

    g_task_stop_requested = false;

    BaseType_t result = xTaskCreate(
        g_selected_task,
        "running_task",
        CMD_TASK_STACK_DEPTH,
        NULL,
        APP_TASKS_SELECTED_PRIORITY,
        &g_selected_task_handle
        );

    if(result != pdPASS)
    {
        ESP_LOGW(APP_TASKS_TAG, "Failed to start task. Aborting");
        g_selected_task_handle = NULL;
    }

    vTaskDelete(NULL);
}

void stop_task(void *param)
{
    if(g_selected_task_handle == NULL)
    {
        ESP_LOGW(APP_TASKS_TAG, "No task is currently running.");
        vTaskDelete(NULL);
        return;
    }

    g_task_stop_requested = true;

    // 2000ms for task cleanup.
    const TickType_t timeout = pdMS_TO_TICKS(2000);
    TickType_t start_time = xTaskGetTickCount();
    
    while(g_selected_task_handle != NULL)
    {
        if(xTaskGetTickCount() - start_time > timeout)
        {
            ESP_LOGW(APP_TASKS_TAG, "Task is unresponsive, enforcing kill.");
            vTaskDelete(g_selected_task_handle);
            g_selected_task_handle = NULL;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(25));
    }

    g_task_stop_requested = false;
    vTaskDelete(NULL);
}

void set_output_task(void *param)
{
    cmd_args_t *args = (cmd_args_t *)param;

    if(args->argc != 1)
    {
        ESP_LOGW(APP_TASKS_TAG, "Expected 1 argument, got %d. Aborting.", args->argc);
        vTaskDelete(NULL);
        return;
    }

    if(strcmp(args->argv[0], "SERIAL") == 0)
    {
        ESP_LOGI(APP_TASKS_TAG, "Switching to Serial");
        log_router_set_target(LOG_OUTPUT_SERIAL);
        ESP_LOGI(APP_TASKS_TAG, "Succesfully changed log output to SERIAL.");
    }
    else if(strcmp(args->argv[0], "UDP") == 0)
    {
        ESP_LOGI(APP_TASKS_TAG, "Switching to UDP");
        log_router_set_target(LOG_OUTPUT_UDP);
        ESP_LOGI(APP_TASKS_TAG, "Succesfully changed log output to UDP. Listen for data on port %d", UDP_DEFAULT_TRANSMIT_PORT);
    }
    else
    {
        ESP_LOGW(APP_TASKS_TAG, "Wrong output specified. Try UDP or SERIAL.");
    }

    vTaskDelete(NULL);
}