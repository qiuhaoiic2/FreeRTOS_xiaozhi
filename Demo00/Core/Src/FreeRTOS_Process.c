#include "FreeRTOS_Process.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"

void Led_Task(void *argument);
void StartDefaultTask(void const *argument)
{
    // 创建一个任务
    xTaskCreate(Led_Task, "Led_Task", 128, NULL, 1, NULL);
    // 启动调度器
    vTaskStartScheduler();
    // 如果调度器启动失败，程序将停在这里
    while (1)
    {
        ;;;;
    }
}

void Led_Task(void *argument)
{
    Led_initialize(&led1);
    Led_initialize(&led2);
    Led_initialize(&led3);
    while (1)
    {
        Led_toggle(&led1);
        Led_toggle(&led2);
        Led_toggle(&led3);
        vTaskDelay(pdMS_TO_TICKS(500)); // 延时500毫秒
    }
}
