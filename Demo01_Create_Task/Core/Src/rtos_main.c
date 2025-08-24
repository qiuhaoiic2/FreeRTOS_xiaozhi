#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"


static void Led_Task(void *argument);
void StartDefaultTask(void const *argument)
{
    xTaskCreate(Led_Task, "Led_Task", 128, &led1, 1, NULL);//argument传递参数的妙用
    xTaskCreate(Led_Task, "Led2_Task", 128, &led2, 1, NULL);//如果把优先级改为2，会发生什么？

    vTaskStartScheduler();
    while (1)
    {
        ;;;;
    }
}
// 演示时间片轮转调度，该任务会500毫秒切换一次LED状态
void Led_Task(void *argument)
{
    gpio_config_t *led = (gpio_config_t *)argument;
    GPIO_Init_Universal(led);
    while (1)
    {
        GPIO_Toggle_Pin(led);
        HAL_Delay(500); // 死亡延时500毫秒
        // vTaskDelay(pdMS_TO_TICKS(500)); // 使用FreeRTOS的延时函数，会导致什么？
    }
}
