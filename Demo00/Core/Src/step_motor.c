#include "step_motor.h"
#include "gpio.h"
gpio_config_t an1 = {
    .GpioPort = GPIOC,
    .Pin = GPIO_PIN_7,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .value = 0};
gpio_config_t an2 = {
    .GpioPort = GPIOC,
    .Pin = GPIO_PIN_8,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .value = 0};
gpio_config_t bn1 = {
    .GpioPort = GPIOC,
    .Pin = GPIO_PIN_9,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .value = 0};
gpio_config_t bn2 = {
    .GpioPort = GPIOA,
    .Pin = GPIO_PIN_8,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .value = 0};

void step_motor_init(void)
{
    GPIO_Init_Universal(&an1);
    GPIO_Init_Universal(&an2);
    GPIO_Init_Universal(&bn1);
    GPIO_Init_Universal(&bn2);
}
void step_motor_run(void)
{
    GPIO_Write_High(&an1);
    GPIO_Write_High(&bn1);
    GPIO_Write_Low(&an2);
    GPIO_Write_Low(&bn2);
    HAL_Delay(1);
    GPIO_Write_Low(&an1);
    GPIO_Write_High(&bn1);
    GPIO_Write_High(&an2);
    GPIO_Write_Low(&bn2);
    HAL_Delay(1);
    GPIO_Write_Low(&an1);
    GPIO_Write_Low(&bn1);
    GPIO_Write_High(&an2);
    GPIO_Write_High(&bn2);
    HAL_Delay(1);
    GPIO_Write_High(&an1);
    GPIO_Write_Low(&bn1);
    GPIO_Write_Low(&an2);
    GPIO_Write_High(&bn2);
    HAL_Delay(1);

}
void step_motor_stop(void)
{
    GPIO_Write_Low(&an1);
    GPIO_Write_Low(&an2);
    GPIO_Write_Low(&bn1);
    GPIO_Write_Low(&bn2);
}
