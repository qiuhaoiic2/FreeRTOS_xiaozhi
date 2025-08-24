#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"

static void led_task(void *para);

#define LED1_STACK_SIZE 64
static StackType_t   Led1_Task_Stack[LED1_STACK_SIZE];//该数组>=任务堆栈大小
static StaticTask_t  Led1_Task_TCB;

#define LED2_STACK_SIZE 64
static StackType_t   Led2_Task_Stack[LED2_STACK_SIZE];//该数组>=任务堆栈大小
static StaticTask_t  Led2_Task_TCB;

TaskHandle_t Led1_Handle = NULL;
TaskHandle_t Led2_Handle = NULL;

void Start_Task()
{
    Led1_Handle = xTaskCreateStatic(led_task,"led1_static",LED1_STACK_SIZE,&led1,1,Led1_Task_Stack,&Led1_Task_TCB);
    if (Led1_Handle == NULL) {;;}
    else               {;;}
    Led2_Handle = xTaskCreateStatic(led_task,"led2_static",LED2_STACK_SIZE,&led2,1,Led2_Task_Stack,&Led2_Task_TCB);
    if (Led2_Handle == NULL) {;;}
    else               {;;}
    vTaskStartScheduler();
    while (1)//如果成功创建了任务，则不会跑到这里来
    {
        ;;
    }
}
static void led_task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    GPIO_Init_Universal(led);
    while (1)
    {
        GPIO_Write_High(led);
        vTaskDelay(pdMS_TO_TICKS(500));
        GPIO_Write_Low(led);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an  
   implementation of vApplicationGetIdleTaskMemory() to provide the memory that is  
   used by the Idle task. */  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,  
                                    StackType_t **ppxIdleTaskStackBuffer,  
                                    uint32_t *pulIdleTaskStackSize )  
{  
    /* If the buffers to be provided to the Idle task are declared inside this  
       function then they must be declared static - otherwise they will be allocated on  
       the stack and so not exists after this function exits. */  
    static StaticTask_t xIdleTaskTCB;  
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];  

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's  
       state will be stored. */  
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;  

    /* Pass out the array that will be used as the Idle task's stack. */  
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;  

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.  
       Note that, as the array is necessarily of type StackType_t,  
       configMINIMAL_STACK_SIZE is specified in words, not bytes. */  
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;  
}  

/*-----------------------------------------------------------*/  

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the  
   application must provide an implementation of vApplicationGetTimerTaskMemory()  
   to provide the memory that is used by the Timer service task. */  
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,  
                                     StackType_t **ppxTimerTaskStackBuffer,  
                                     uint32_t *pulTimerTaskStackSize )  
{  
    /* If the buffers to be provided to the Timer task are declared inside this  
       function then they must be declared static - otherwise they will be allocated on  
       the stack and so not exists after this function exits. */  
    static StaticTask_t xTimerTaskTCB;  
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];  

    /* Pass out a pointer to the StaticTask_t structure in which the Timer  
       task's state will be stored. */  
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;  

    /* Pass out the array that will be used as the Timer task's stack. */  
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;  

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.  
       Note that, as the array is necessarily of type StackType_t,  
      configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */  
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;  
}  

