/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "led.h"
#include "spi.h"
#include "oled.h"
#include "i2c.h"
#include "temperature.h"
#include "key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
float temperature = 0.0f;
float humidity = 0.0f;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */

void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  oled_initialize();
  gxht3l_init();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  uint8_t cnt = 0, key_value = KEY_NONE;
  GPIO_Init_Universal(&led1);
  gpio_config_t *keys[3] = {&key1, &key2, &key3};
  uint8_t key_lines[3] = {3, 4, 5};
  char key_char = 'O';

  for (cnt = 0; cnt < 3; cnt++)
  {
    GPIO_Init_Universal(keys[cnt]);
  }
  // 显示固定的标题和标签
  oled_show_string(1, 4, "Screen Content:", 6);
  oled_show_string(3, 2, "Temp: ", 6);
  oled_show_string(4, 2, "Humi: ", 6);

  // 显示按键状态标签
  oled_show_string(3, 15, "Key1: *", 6);
  oled_show_string(4, 15, "Key2: *", 6);
  oled_show_string(5, 15, "Key3: *", 6);

  for (;;)
  {
    // 获取温湿度数据
    gxht3l_get_temp_humi(&temperature, &humidity);

    // 显示温度值（在"Temp: "后面）
    oled_show_temperature(3, 7, temperature, 6);

    // 显示湿度值（在"Humi: "后面）
    oled_show_humidity(4, 7, humidity, 6);

    for (cnt = 0; cnt < 3; cnt++)
    {
      key_value = key_is_pressed(keys[cnt]);

      if (key_value == KEY_PRESS)
      {
        key_char = '*';
      }
      else if (key_value == KEY_RELEASE)
      {
        key_char = 'O';
      }
      oled_show_char(key_lines[cnt], 21, key_char, 6);
    }

    GPIO_Toggle_Pin(&led1);

    HAL_Delay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
