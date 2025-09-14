#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "led.h"
#include "key.h"

static void low_task(void *para);
static void mid_task(void *para);
static void high_task(void *para);




SemaphoreHandle_t xMutex; // 互斥量（演示优先级继承）

void Start_Task(void)
{
    /* 创建互斥量 */
    //  xMutex = xSemaphoreCreateMutex();
    xMutex = xSemaphoreCreateBinary();
    configASSERT(xMutex != NULL);

    /* 故意拉开优先级：低 → 中 → 高 */
    xTaskCreate(low_task, "low_task", 128, NULL, 1, NULL);   // 1：低
    xTaskCreate(mid_task, "mid_task", 128, NULL, 2, NULL);   // 2：中
    xTaskCreate(high_task, "high_task", 128, NULL, 3, NULL); // 3：高
    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
    {
        ;
    }
}
static void low_task(void *pv)
{
    while (1)
    {
        printf("low_task: trying to get mutex...\n");
        xSemaphoreTake(xMutex, portMAX_DELAY);
        printf("low_task: start hold UART 3 s\n");

        /* 故意阻塞 3 秒，制造反转 */
        vTaskDelay(pdMS_TO_TICKS(3000));

        printf("low_task: release UART\n");
        xSemaphoreGive(xMutex);
        
        /* 给其他任务一些时间 */
        vTaskDelay(pdMS_TO_TICKS(2000));  // 等待2秒再次尝试
    }
}
static void mid_task(void *pv)
{
    while (1)
    {
        printf("mid_task: running...\n");
        vTaskDelay(pdMS_TO_TICKS(500));   // 不停抢 CPU
    }
}
static void high_task(void *pv)
{
    /* 延时更长时间，让 low_task 先获得锁 */
    vTaskDelay(pdMS_TO_TICKS(1500));  // 改为1.5秒
    
    while (1)
    {
        printf("high_task: wait mutex...\n");
        
        /* 拿锁（应该会被低优先级阻塞，触发优先级继承） */
        xSemaphoreTake(xMutex, portMAX_DELAY);
        printf("high_task: got lock!\n");
        
        /* 极短临界区 */
        vTaskDelay(pdMS_TO_TICKS(100));
        xSemaphoreGive(xMutex);
        
        /* 添加延时，给其他任务机会 */
        vTaskDelay(pdMS_TO_TICKS(5000));  // 5秒后再次尝试
    }
}
