#include "dc_motor.h"
#include "gpio.h"
#include "tim.h"
gpio_config_t dc_motor_gpio = {
    .GpioPort = GPIOB,
    .Pin = GPIO_PIN_14,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .value = 0
};

void dc_motor_init(void)
{
    GPIO_Init_Universal(&dc_motor_gpio);
    dc_motor_set_speed(0); // 初始化时停止电机
    GPIO_Write_Low(&dc_motor_gpio); // 设置电机方向
}

void dc_motor_set_speed(uint16_t speed)
{
    if (speed > 500)
        speed = 500; // 限制最大速度为500
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,speed);

}

void dc_motor_stop(void)
{
    __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,0);
}
