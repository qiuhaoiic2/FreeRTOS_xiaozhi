#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "led.h"
#include "key.h"

static void log_task(void *para);
static void Led_Task(void *para);
static void Key_Task(void *para);
QueueHandle_t Binary_Semaphore = NULL;

void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);

    Binary_Semaphore = xSemaphoreCreateBinary();
    configASSERT(Binary_Semaphore != NULL);

    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(Led_Task, "Led1_Task", 128, &led1, 2, NULL);
    xTaskCreate(Key_Task, "Key1_Task", 128, &key1, 3, NULL);
    xTaskCreate(Key_Task, "Key2_Task", 128, &key2, 3, NULL);
    xTaskCreate(Key_Task, "Key3_Task", 128, &key3, 3, NULL);

    vTaskStartScheduler();
    while (1) // 如果成功创建了任务，则不会跑到这里来
    {
        ;
        ;
    }
}
static void Led_Task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;

    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    while (1)
    {
        xSemaphoreTake(Binary_Semaphore, portMAX_DELAY);//阻塞等待，取出信号量，并将信号量置为0

        log_print("%s get Binary Semaphore", pcTaskGetName(NULL));
        GPIO_Toggle_Pin(led);

    }
}

static void Key_Task(void *para)
{
    gpio_config_t *key = (gpio_config_t *)para;
    key_event_t key_value = KEY_NONE;
    GPIO_Init_Universal(key);
    while (1)
    {
        key_value = key_scan(key);
        if (key_value == KEY_SHORT)
        {
            xSemaphoreGive(Binary_Semaphore);
            log_print("%s give Binary Semaphore", pcTaskGetName(NULL));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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
