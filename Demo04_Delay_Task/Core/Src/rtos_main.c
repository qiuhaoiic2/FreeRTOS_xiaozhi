#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TICKS_TO_MS(ticks) ((ticks)*portTICK_PERIOD_MS)

static void log_task(void *para);
static void delay_task(void *para);
static void until_task(void *para);



TaskHandle_t Led1_Handle = NULL;
TaskHandle_t Led2_Handle = NULL;
void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);
    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(delay_task, "delay_task", 128, &led1, 2, &Led1_Handle);
    xTaskCreate(until_task, "until_task", 128, &led2, 2, &Led2_Handle);

    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
    {
        ;
        ;
    }
}
static void delay_task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    TickType_t last_wake_time = 0;
    last_wake_time = xTaskGetTickCount();
    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    while (1)
    {
        GPIO_Toggle_Pin(led);
        HAL_Delay(400);// 模拟任务处理时间
        vTaskDelay(pdMS_TO_TICKS(500));
        log_print("Task1 Delay process time = %d ms",TICKS_TO_MS(xTaskGetTickCount() - last_wake_time));
        last_wake_time = xTaskGetTickCount(); // 更新 last_wake_time
    }
}
static void until_task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    TickType_t last_wake_time = 0;
    TickType_t current_time = 0;
    last_wake_time = xTaskGetTickCount();

    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    while (1)
    {
        GPIO_Toggle_Pin(led);
        HAL_Delay(400);// 模拟任务处理时间
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(500)); //延时500ms，并且更新last_wake_time
        log_print("Task2 Until process time = %d ms", TICKS_TO_MS(current_time - last_wake_time));
        last_wake_time = current_time; // 更新 last_wake_time
    }
}

static void log_task(void *para)
{
    char message[LOG_MAX_LEN];
    while (1)
    {
        if (xQueueReceive(xLogQueue, message, portMAX_DELAY) == pdPASS)
        {
            printf("%s\r\n", message);
        }
    }
}
