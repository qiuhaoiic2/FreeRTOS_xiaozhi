#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"

#include "stdbool.h"

typedef struct {
    GPIO_TypeDef *GpioPort;   // GPIO 端口（GPIOA、GPIOB...）
    uint32_t Pin;             // GPIO 引脚（GPIO_PIN_0...）
    uint32_t Mode;            // 模式（输入/输出/复用...）
    uint32_t Pull;            // 上拉/下拉（GPIO_NOPULL, GPIO_PULLUP...）
    uint32_t Speed;           // 速度（GPIO_SPEED_FREQ_LOW...）
    bool     value;           // 按键读取的value
} gpio_config_t;




int8_t GPIO_Init_Universal(gpio_config_t *config);
int8_t GPIO_Write_High(gpio_config_t *config);
int8_t GPIO_Write_Low(gpio_config_t *config);
int8_t GPIO_Toggle_Pin(gpio_config_t *config);

int8_t GPIO_Read_Pin(gpio_config_t *config);




#endif /*__ GPIO_H__ */

