#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "exit.h"

static void log_task(void *para);



QueueHandle_t Task_TO_Interrupt_Binary;
QueueHandle_t Interrupt_TO_Task_Binary;

// uint8_t     Led_Status[3] = {1,1,1};


void Start_Task()
{
    int8_t res = -1;
    log_init();
    configASSERT(xLogQueue != NULL);
    res = GPIO_Init_Universal(&key1);
    configASSERT(res != -1);
    res = Exti_Initialize(&key1);
    configASSERT(res != -1);

    Task_TO_Interrupt_Binary = xSemaphoreCreateBinary();
    Interrupt_TO_Task_Binary = xSemaphoreCreateBinary();

    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);

    xTaskCreate(Led_Task, "Led_Task", 128, NULL, 2, NULL);

    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
    {
        ;
        
    }
}
static void Led_Task(void *para)
{

    uint8_t cnt = 0;
    GPIO_Init_Universal(&led1);
    GPIO_Init_Universal(&led2);
    GPIO_Init_Universal(&led3);
    GPIO_Write_Low(&led2);
    while (1)
    {
        xSemaphoreTake(Interrupt_TO_Task_Binary,portMAX_DELAY);
        GPIO_Write_High(&led2);
        for (cnt = 0;cnt < 4;cnt++)
        {
            GPIO_Toggle_Pin(&led1);
            vTaskDelay(100);
        }
        GPIO_Write_High(&led1);

        xSemaphoreGive(Task_TO_Interrupt_Binary);
        log_print("task process ending and wait next interrupt");
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


void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 判断 & 清中断标志 */
    if (__HAL_GPIO_EXTI_GET_IT(key1.Pin) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(key1.Pin);
        xSemaphoreTakeFromISR(Interrupt_TO_Task_Binary,xHigherPriorityTaskWoken);
        log_print("interrupt event occur wait for task process...");
        xSemaphoreGiveFromISR(Task_TO_Interrupt_Binary,xHigherPriorityTaskWoken);
        GPIO_Write_Low(&led2);

    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
