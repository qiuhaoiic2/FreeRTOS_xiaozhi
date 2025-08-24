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

QueueHandle_t Counting_Semaphore = NULL;

void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);

    Counting_Semaphore = xSemaphoreCreateCounting(3, 0); // 总共给了三把钥匙，初始化为0把
    configASSERT(Counting_Semaphore != NULL);

    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(Led_Task, "Led1_Task", 128, &led1, 2, NULL);
    xTaskCreate(Led_Task, "Led2_Task", 128, &led2, 2, NULL);
    xTaskCreate(Led_Task, "Led3_Task", 128, &led3, 2, NULL);

    xTaskCreate(Key_Task, "Key1_Task", 128, &key1, 3, NULL);

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
        xSemaphoreTake(Counting_Semaphore, portMAX_DELAY);
        log_print("%s get Counting Semaphore", pcTaskGetName(NULL));
        GPIO_Toggle_Pin(led);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void Key_Task(void *para)
{
    gpio_config_t *key = (gpio_config_t *)para;
    key_event_t key_value = KEY_NONE;
    uint8_t cnt = 0;
    GPIO_Init_Universal(key);
    while (1)
    {
        key_value = key_scan(key);
        if (key_value == KEY_SHORT)
        {
            for (cnt = 0; cnt < 3; cnt++) // 一个按键控制三个LED
            {
                xSemaphoreGive(Counting_Semaphore);
            }
            log_print("%s give Counting Semaphore", pcTaskGetName(NULL));
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
