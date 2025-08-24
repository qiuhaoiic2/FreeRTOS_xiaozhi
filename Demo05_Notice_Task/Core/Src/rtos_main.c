#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h" 
#include "key.h"

static void log_task(void *para);
static void Led_Task(void *para);
static void Key_Task(void *para);
TaskHandle_t Led1_Handle = NULL;
TaskHandle_t Led2_Handle = NULL;
TaskHandle_t Led3_Handle = NULL;

void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);
    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(Led_Task, "Led1_Task", 128, &led1, 2, &Led1_Handle);
    xTaskCreate(Led_Task, "Led2_Task", 128, &led2, 2, &Led2_Handle);
    xTaskCreate(Led_Task, "Led3_Task", 128, &led3, 2, &Led3_Handle);
    xTaskCreate(Key_Task, "Key1_Task", 128, &key1, 3, NULL);
    xTaskCreate(Key_Task, "Key2_Task", 128, &key2, 3, NULL);
    xTaskCreate(Key_Task, "Key3_Task", 128, &key3, 3, NULL);

    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
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
      ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
      log_print("%s get notice",pcTaskGetName(NULL));
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
          if (key == &key1) xTaskNotifyGive(Led1_Handle);
          if (key == &key2) xTaskNotifyGive(Led2_Handle);
          if (key == &key3) xTaskNotifyGive(Led3_Handle);
          log_print("%s give notice",pcTaskGetName(NULL));
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
