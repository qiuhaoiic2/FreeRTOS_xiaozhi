#include "FreeRTOS_Process.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"

void Led_Task(void *argument);
void StartDefaultTask(void const *argument)
{
    // ����һ������
    xTaskCreate(Led_Task, "Led_Task", 128, NULL, 1, NULL);
    // ����������
    vTaskStartScheduler();
    // �������������ʧ�ܣ�����ͣ������
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
        vTaskDelay(pdMS_TO_TICKS(500)); // ��ʱ500����
    }
}
