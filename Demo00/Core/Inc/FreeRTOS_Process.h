#ifndef __FREERTOS_PROCESS_H__
#define __FREERTOS_PROCESS_H__

#include "main.h"

void StartDefaultTask(void const *argument);

void Led_Task(void *argument);

#endif /* __FREERTOS_PROCESS_H__ */