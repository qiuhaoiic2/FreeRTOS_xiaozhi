#include "gpio.h"

int8_t GPIO_Init_Universal(gpio_config_t *config)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (config == NULL) return -1;

  if (config->GpioPort == GPIOA)
    __HAL_RCC_GPIOA_CLK_ENABLE();
  else if (config->GpioPort == GPIOB)
    __HAL_RCC_GPIOB_CLK_ENABLE();
  else if (config->GpioPort == GPIOC)
    __HAL_RCC_GPIOC_CLK_ENABLE();
  else if (config->GpioPort == GPIOD)
    __HAL_RCC_GPIOD_CLK_ENABLE();
  else if (config->GpioPort == GPIOE)
    __HAL_RCC_GPIOE_CLK_ENABLE();
  else 
    return -1;
  GPIO_InitStruct.Pin = config->Pin;
  GPIO_InitStruct.Mode = config->Mode;
  GPIO_InitStruct.Pull = config->Pull;
  GPIO_InitStruct.Speed = config->Speed;
  HAL_GPIO_Init(config->GpioPort, &GPIO_InitStruct);
  return 0;
}
int8_t GPIO_Write_High(gpio_config_t *config)
{
  if (config == NULL) return -1;
  HAL_GPIO_WritePin(config->GpioPort,config->Pin,GPIO_PIN_SET);
  return 0;

}

int8_t GPIO_Write_Low(gpio_config_t *config)
{
  if (config == NULL) return -1;
  HAL_GPIO_WritePin(config->GpioPort,config->Pin,GPIO_PIN_RESET);
  return 0;

}
int8_t GPIO_Toggle_Pin(gpio_config_t *config)
{
  if (config == NULL) return -1;
  HAL_GPIO_TogglePin(config->GpioPort,config->Pin);
  return 0;
}



