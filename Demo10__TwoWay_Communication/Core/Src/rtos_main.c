#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "exit.h"
#include "key.h"
#include "led.h"

// 任务句柄
static TaskHandle_t xLedTaskHandle = NULL;

// 静态函数声明
static void log_task(void *para);
static void Led_Task(void *para);

// 信号量句柄
QueueHandle_t Task_TO_Interrupt_Binary;
QueueHandle_t Interrupt_TO_Task_Binary;

// 统计信息结构体
typedef struct {
    uint32_t interrupt_count;
    uint32_t task_process_count;
    uint32_t communication_errors;
    TickType_t last_interrupt_time;
    TickType_t avg_process_time;
} communication_stats_t;

static communication_stats_t stats = {0};

void Start_Task()
{
    int8_t res = -1;
    
    // 初始化日志系统
    log_init();
    configASSERT(xLogQueue != NULL);
    
    // 初始化外部中断
    res = Exti_Initialize(&key1);
    configASSERT(res != -1);

    // 创建二进制信号量
    Task_TO_Interrupt_Binary = xSemaphoreCreateBinary();
    Interrupt_TO_Task_Binary = xSemaphoreCreateBinary();
    configASSERT(Task_TO_Interrupt_Binary != NULL);
    configASSERT(Interrupt_TO_Task_Binary != NULL);
    
    // 初始状态：任务准备就绪
    xSemaphoreGive(Task_TO_Interrupt_Binary);

    // 创建任务
    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(Led_Task, "Led_Task", 256, NULL, 3, &xLedTaskHandle);

    log_print("System initialized, waiting for interrupts...");
    
    vTaskStartScheduler();
    
    // 不应该到达这里
    while (1) {
        ;
    }
}

static void Led_Task(void *para)
{
    uint8_t process_count = 0;
    TickType_t start_time, end_time;
    
    // 初始化LED
    GPIO_Init_Universal(&led1);
    GPIO_Init_Universal(&led2);
    GPIO_Write_High(&led1);    // LED1初始关闭
    GPIO_Write_Low(&led2);     // LED2作为状态指示
    
    log_print("Led_Task started, ready for interrupts");
    
    while (1)
    {
        // 等待中断信号
        if (xSemaphoreTake(Interrupt_TO_Task_Binary, portMAX_DELAY) == pdTRUE)
        {
            start_time = xTaskGetTickCount();
            
            // 处理中断事件
            process_count++;
            stats.task_process_count++;
            
            log_print("Processing interrupt #%d", process_count);
            
            // LED状态指示：处理中
            GPIO_Write_High(&led2);
            
            // 模拟处理过程
            for (uint8_t i = 0; i < 3; i++) {
                GPIO_Toggle_Pin(&led1);
                vTaskDelay(pdMS_TO_TICKS(800));
            }
            
            // 计算处理时间
            end_time = xTaskGetTickCount();
            stats.avg_process_time = (stats.avg_process_time + (end_time - start_time)) / 2;
            
            // LED状态指示：处理完成
            GPIO_Write_Low(&led2);
            
            // 通知中断：任务处理完成，可以接受下一个中断
            if (xSemaphoreGive(Task_TO_Interrupt_Binary) == pdTRUE) {
                log_print("Task processing completed, ready for next interrupt");
            } else {
                stats.communication_errors++;
                log_print("ERROR: Failed to signal task completion");
            }
        }
    }
}

static void log_task(void *para)
{
    char message[LOG_MAX_LEN];
    
    while (1)
    {
        if (xQueueReceive(xLogQueue, message, portMAX_DELAY) == pdPASS)
        {
            printf("[%lu] %s\r\n", xTaskGetTickCount() * portTICK_PERIOD_MS, message);
        }
    }
}

// 优化后的中断处理函数
void EXTI3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    static TickType_t last_interrupt_time = 0;
    TickType_t current_time = xTaskGetTickCountFromISR();
    
    /* 判断 & 清中断标志 */
    if (__HAL_GPIO_EXTI_GET_IT(key1.Pin) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(key1.Pin);
        
        // 防抖处理：如果距离上次中断时间太短，忽略
        if (current_time - last_interrupt_time < pdMS_TO_TICKS(50)) {
            return;
        }
        
        last_interrupt_time = current_time;
        stats.last_interrupt_time = current_time;
        
        // 尝试获取任务状态信号量（检查任务是否准备好）
        if (xSemaphoreTakeFromISR(Task_TO_Interrupt_Binary, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            // 任务准备就绪，发送处理信号
            stats.interrupt_count++;
            xSemaphoreGiveFromISR(Interrupt_TO_Task_Binary, &xHigherPriorityTaskWoken);
        }
        else
        {
            // 任务繁忙，记录错误
            stats.communication_errors++;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
