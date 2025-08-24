#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void led1_task(void *para);
static void led2_task(void *para);
static void log_task (void *para);


TaskHandle_t Led1_Handle = NULL;
TaskHandle_t Led2_Handle = NULL;
void Start_Task()
{
    log_init();
    xTaskCreate(log_task,"log_task",256,NULL,1,NULL);
    xTaskCreate(led1_task,"led1_task",128,&led1,1,&Led1_Handle);
    xTaskCreate(led2_task,"led2_task",128,&led2,1,&Led2_Handle);

    vTaskStartScheduler();
    while (1)//如果成功创建了任务，则不会跑到这里来
    {
        ;;
    }
}
static void led1_task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    uint8_t cnt = 0;
    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    while (1)
    {
        GPIO_Toggle_Pin(led);
        cnt ++;
        if (4 == cnt)
        {
            log_print("task1 suspend");
            vTaskSuspend(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(500));

    }
    
}
static void led2_task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    uint8_t cnt = 0;
    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    while (1)
    {
        GPIO_Toggle_Pin(led);
        cnt ++;
        if(8 == cnt)
        {
            log_print("task1 resume");
            vTaskResume(Led1_Handle);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));

    }   
    log_print("task2 delete");
    Led2_Handle = NULL;
    vTaskDelete(NULL);
}



static void log_task(void *para)
{
    char message[LOG_MAX_LEN];
    while (1)  
    {
        if (xQueueReceive(xLogQueue,message,portMAX_DELAY) == pdPASS)
        {
            printf("%s\r\n",message);
        }
    }
}

