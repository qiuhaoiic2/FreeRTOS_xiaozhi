#ifndef __UART_H
#define __UART_H

#include "main.h"

extern UART_HandleTypeDef huart1;
int8_t Uart1_Initialize(uint32_t baudrate);


#endif
