#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "exit.h"
static void low_task(void *para);
static void mid_task(void *para);
static void high_task(void *para);




SemaphoreHandle_t xMutex; // 互斥量（演示优先级继承）

void Start_Task(void)
{
    /* 创建互斥量 */
    // xMutex = xSemaphoreCreateMutex();
    xMutex = xSemaphoreCreateBinary();
    configASSERT(xMutex != NULL);
    xSemaphoreGive(xMutex);

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
        xSemaphoreTake(xMutex, portMAX_DELAY);
        printf("low_task: start hold UART 3 s\n");

        /* 故意阻塞 3 秒，制造反转 */
        vTaskDelay(pdMS_TO_TICKS(3000));

        printf("low_task: release UART\n");
        xSemaphoreGive(xMutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1)
    {
        printf("high_task: wait mutex...\n");

        /* 拿锁（会被低优先级阻塞） */
        xSemaphoreTake(xMutex, portMAX_DELAY);
        printf("high_task: got lock!\n");

        /* 极短临界区 */
        vTaskDelay(pdMS_TO_TICKS(100));
        xSemaphoreGive(xMutex);
    }
}
