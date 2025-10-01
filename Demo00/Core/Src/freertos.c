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
#include "oled.h"
#include "temperature.h"
#include "key.h"
#include "dc_motor.h"
#include "step_motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    float temperature;
    float humidity;
} sensor_msg_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 按键事件通知位（按下/松开）
#define KEY1_PRESSED (1U << 0)
#define KEY1_RELEASED (1U << 3)
#define KEY2_PRESSED (1U << 1)
#define KEY2_RELEASED (1U << 4)
#define KEY3_PRESSED (1U << 2)
#define KEY3_RELEASED (1U << 5)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
float temperature = 0.0f;
float humidity = 0.0f;
osThreadId_t key_task_handle;
const osThreadAttr_t keyTask_attributes = {
    .name = "keyTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityHigh,
};
osThreadId_t dip_task_handle;
const osThreadAttr_t dipTask_attributes = {
    .name = "dipTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

osThreadId_t env_collect_task_handle;
const osThreadAttr_t envCollectTask_attributes = {
    .name = "envCollectTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

osThreadId_t control_task_handle;
const osThreadAttr_t controlTask_attributes = {
    .name = "controlTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal1,
};
osMessageQueueId_t sensor_queue;

/* USER CODE END Variables */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
void env_collect_task(void *arg);
void dip_task(void *arg);
void key_task(void *arg);
void control_task(void *arg);
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
    dc_motor_init();
    step_motor_init();
    GPIO_Init_Universal(&led1);
    GPIO_Init_Universal(&key1);
    GPIO_Init_Universal(&key2);
    GPIO_Init_Universal(&key3);
    // 显示固定的标题和标签
    oled_show_string(1, 4, "Screen Content:", 12);
    oled_show_string(2, 2, "Temp: ", 12);
    oled_show_string(4, 2, "Humi: ", 12);

    // 显示按键状态标签
    oled_show_string(2, 15, "Key1: *", 12);
    oled_show_string(3, 15, "Key2: *", 12);
    oled_show_string(4, 15, "Key3: *", 12);
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
    sensor_queue = osMessageQueueNew(4, sizeof(sensor_msg_t), NULL); /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */

    /* USER CODE BEGIN RTOS_THREADS */
    key_task_handle = osThreadNew(key_task, NULL, &keyTask_attributes);
    dip_task_handle = osThreadNew(dip_task, NULL, &dipTask_attributes);
    env_collect_task_handle = osThreadNew(env_collect_task, NULL, &envCollectTask_attributes);
    control_task_handle = osThreadNew(control_task, NULL, &controlTask_attributes);

    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void key_task(void *arg)
{
    /* Infinite loop */
    uint8_t cnt = 0, key_value = KEY_NONE;
    gpio_config_t *keys[3] = {&key1, &key2, &key3};

    while(1)
    {
        uint32_t notify = 0;

        for (cnt = 0; cnt < 3; cnt++)
        {
            key_value = key_is_pressed(keys[cnt]);

            if (key_value == KEY_PRESS)
            {
                if (cnt == 0)      notify |= KEY1_PRESSED;
                else if (cnt == 1) notify |= KEY2_PRESSED;
                else               notify |= KEY3_PRESSED;
            }
            else if (key_value == KEY_RELEASE)
            {
                if (cnt == 0)      notify |= KEY1_RELEASED;
                else if (cnt == 1) notify |= KEY2_RELEASED;
                else               notify |= KEY3_RELEASED;
            }
        }

        if (notify) {
            osThreadFlagsSet(dip_task_handle, notify); // 通知显示任务
            osThreadFlagsSet(control_task_handle, notify);   // 通知控制任务
        }
        osDelay(10);
    }
}
void dip_task(void *arg)
{
    sensor_msg_t msg;
    uint32_t flags;

    while(1)
    {
        // 队列：非阻塞获取温湿度并显示
        if (osMessageQueueGet(sensor_queue, &msg, NULL, 0) == osOK) {
            oled_show_temperature(2, 7, msg.temperature, 12);
            oled_show_humidity(4, 7, msg.humidity, 12);
        }

        // 任务通知：按键状态显示
        flags = osThreadFlagsWait(KEY1_PRESSED | KEY1_RELEASED |
                                  KEY2_PRESSED | KEY2_RELEASED |
                                  KEY3_PRESSED | KEY3_RELEASED,
                                  osFlagsWaitAny, 0);
        if ((int32_t)flags >= 0) {
            if (flags & KEY1_PRESSED)  oled_show_char(2, 21, '*', 12);
            if (flags & KEY1_RELEASED) oled_show_char(2, 21, 'O', 12);

            if (flags & KEY2_PRESSED)  oled_show_char(3, 21, '*', 12);
            if (flags & KEY2_RELEASED) oled_show_char(3, 21, 'O', 12);

            if (flags & KEY3_PRESSED)  oled_show_char(4, 21, '*', 12);
            if (flags & KEY3_RELEASED) oled_show_char(4, 21, 'O', 12);
        }

        osDelay(1);
    }
}
void env_collect_task(void *arg)
{
    sensor_msg_t msg;
    while(1)
    {
        if (gxht3l_get_temp_humi(&temperature, &humidity) == GXHT3L_OK) {
            msg.temperature = temperature;
            msg.humidity = humidity;
            osMessageQueuePut(sensor_queue, &msg, 0U, 0U);
        }
        osDelay(1000);
    }
}
void control_task(void *arg)
{
    while(1)
    {
        uint32_t flags = osThreadFlagsWait(
            KEY1_PRESSED | KEY1_RELEASED |
            KEY2_PRESSED | KEY2_RELEASED |
            KEY3_PRESSED | KEY3_RELEASED,
            osFlagsWaitAny, osWaitForever);

        // Key1：按下灯亮，松开灯灭
        if (flags & KEY1_PRESSED)  GPIO_Write_Low(&led1);
        if (flags & KEY1_RELEASED) GPIO_Write_High(&led1);

        // Key2：按下步进电机转动，松开停止
        taskENTER_CRITICAL();
        if (flags & KEY2_PRESSED)  step_motor_run();
        taskEXIT_CRITICAL();
        if (flags & KEY2_RELEASED) step_motor_stop();

        // Key3：按下直流电机转动，松开停止
        if (flags & KEY3_PRESSED)  dc_motor_set_speed(499);  
        if (flags & KEY3_RELEASED) dc_motor_stop();
    }
}

/* USER CODE END Application */
