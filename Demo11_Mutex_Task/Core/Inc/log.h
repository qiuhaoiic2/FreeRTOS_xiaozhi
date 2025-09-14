/* log.h ----------------------------------------------------*/
#ifndef LOG_H
#define LOG_H

#include "main.h"


#define LOG_MAX_LEN   128
#define LOG_QUEUE_LEN 32

extern QueueHandle_t xLogQueue = NULL;


QueueHandle_t log_init(void);
BaseType_t log_print(const char *fmt, ...);


#endif