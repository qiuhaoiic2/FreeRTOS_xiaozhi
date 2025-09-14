#include "exit.h"
#include "stm32f1xx_hal_exti.h"

int8_t Exti_Initialize(gpio_config_t *gpio)
{
    IRQn_Type irqn;
    uint32_t pin = gpio->Pin;
    if (gpio == NULL || gpio->Mode != GPIO_MODE_IT_FALLING)
        return -1;

    /* 1. 初始化引脚（下降沿 + 上拉） */
    GPIO_Init_Universal(gpio);

    /* 2. 根据引脚号计算 EXTI 线并配置 NVIC */
    if      (pin == GPIO_PIN_0)        irqn = EXTI0_IRQn;
    else if (pin == GPIO_PIN_1)        irqn = EXTI1_IRQn;
    else if (pin == GPIO_PIN_2)        irqn = EXTI2_IRQn;
    else if (pin == GPIO_PIN_3)        irqn = EXTI3_IRQn;
    else if (pin == GPIO_PIN_4)        irqn = EXTI4_IRQn;
    else if (pin >= GPIO_PIN_5 && pin <= GPIO_PIN_9)      irqn = EXTI9_5_IRQn;
    else if (pin >= GPIO_PIN_10 && pin <= GPIO_PIN_15)    irqn = EXTI15_10_IRQn;
    else return -2;

    HAL_NVIC_SetPriority(irqn, 5, 0);
    HAL_NVIC_EnableIRQ(irqn);

    return 0;
}
