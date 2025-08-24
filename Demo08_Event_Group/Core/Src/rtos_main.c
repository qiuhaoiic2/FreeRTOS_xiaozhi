#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "led.h"
#include "key.h"

static void log_task(void *para);
static void Led_Task(void *para);
static void Key_Task(void *para);


EventGroupHandle_t Event_Groups_Handle = NULL;

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);

    Event_Groups_Handle = xEventGroupCreate();
    configASSERT(Event_Groups_Handle != NULL);

    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);

    xTaskCreate(Led_Task, "Led_Task", 128, NULL, 2, NULL);
    xTaskCreate(Key_Task, "Key_Task", 128, NULL, 3, NULL);

    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
    {
        ;
        ;
    }
}
static void Led_Task(void *para)
{
    EventBits_t result_event = 0;
    GPIO_Init_Universal(&led1);
    GPIO_Init_Universal(&led2);
    GPIO_Init_Universal(&led3);
    GPIO_Write_High(&led1);
    GPIO_Write_High(&led2);
    GPIO_Write_High(&led3);
    while (1)
    {
        // 第一个 pdtrue --> 用完之后清零
        // 第二个 pdfalse --> 是任意一个位发生都会返回
        result_event = xEventGroupWaitBits(Event_Groups_Handle, BIT_0 | BIT_1 | BIT_2,
                                           pdTRUE, pdFALSE, portMAX_DELAY);
        log_print("led task get event: 0x%02X", result_event);
        if (result_event == BIT_0)
        {
            GPIO_Toggle_Pin(&led1);
        }
        else if (result_event == BIT_1)
        {
            GPIO_Toggle_Pin(&led2);
        }
        else if (result_event == BIT_2)
        {
            GPIO_Toggle_Pin(&led3);
        }
    }
}

static void Key_Task(void *para)
{
    key_event_t key_value = KEY_NONE;
    uint8_t cnt = 0;
    gpio_config_t *keys[3] = {&key1, &key2, &key3};
    for (cnt = 0; cnt < 3; cnt++)
    {
        GPIO_Init_Universal(keys[cnt]);
    }

    while (1)
    {
        for (cnt = 0; cnt < 3; cnt++)
        {
            key_value = key_scan(keys[cnt]);
            if (key_value == KEY_SHORT)
            {
                xEventGroupSetBits(Event_Groups_Handle,1 << cnt);//优化后的写法
                log_print("key %d short press and set bit %d", cnt + 1, 1 << cnt);
                break;
            }
        }
        // switch (cnt)//优化前的写法
        // {
        // case 0:
        //     xEventGroupSetBits(Event_Groups_Handle, BIT_0);
        //     break;
        // case 1:
        //     xEventGroupSetBits(Event_Groups_Handle, BIT_1);
        //     break;
        // case 2:
        //     xEventGroupSetBits(Event_Groups_Handle, BIT_2);
        //     break;
        // }
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
