#include "tim3.h"

TIM_HandleTypeDef htim3;

void TIM3_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = 72 - 1;        // 72 MHz / 72 = 1 MHz
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.Period            = 1000 - 1;      // 1 MHz / 1000 = 1 kHz → 1 ms
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
			;;;
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

    /* 使能更新中断 & 启动 */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();                 // 时钟

        HAL_NVIC_SetPriority(TIM3_IRQn, 7, 0);       // 优先级
        HAL_NVIC_EnableIRQ(TIM3_IRQn);               // 使能中断
    }
}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}



