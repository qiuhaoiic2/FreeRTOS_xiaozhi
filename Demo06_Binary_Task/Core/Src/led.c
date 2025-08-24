#include "led.h"
// LED 配置（推挽输出）

gpio_config_t led1 = {
    .GpioPort = GPIOA,
    .Pin = GPIO_PIN_6,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_LOW
};

gpio_config_t led2 = {
    .GpioPort = GPIOA,
    .Pin = GPIO_PIN_5,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_LOW
};  

gpio_config_t led3 = {
    .GpioPort = GPIOA,
    .Pin = GPIO_PIN_4,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_LOW
};
