#include "log.h"
#include "FreeRTOS.h"
#include "queue.h"



QueueHandle_t xLogQueue = NULL;

void log_init(void)
{
    xLogQueue = xQueueCreate(LOG_QUEUE_LEN, LOG_MAX_LEN);
    
}

/* 业务任务调用：非阻塞发送 */
BaseType_t log_print(const char *fmt, ...)
{
    BaseType_t res = pdFAIL;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    /* 0 表示队列满直接丢日志，不阻塞业务任务 */
    res = xQueueSendToBack(xLogQueue, buf, 0);
    return res;
}
