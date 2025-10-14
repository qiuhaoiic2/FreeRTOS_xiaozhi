# 本文档是说明小智开发板的FreeRTOS的说明


## 创建工程
1. 使用的是cubemx生成FreeRTOS模板（这里建议先用裸机测试一下自己的工程是否有问题，可以简单的点灯）
2. 创建的时候，有几个需要注意的点：如果需要使用HAL_Delay()等API，需要手动将时钟替换成非Systick的一个定时器，我这里使用的是TIM2。
3. 创建成功后，可以继续点灯测试工程是否有问题（建立创建两个不同的LED闪烁任务），测试osDelay是否正常，和切换任务是否正常

## 裸机实现代码
1. 相信大家开始学FreeRTOS之前，已经写过STM32各种外设的代码了，所以后续的代码不再使用cubemx生成，而是采用模块编程的思想，将之前写的代码一步步的移植到当前工程，建议这一步移植一个就测试一个功能。
2. 等各个模块都移植进来了，再使用一个任务来完成课后Demo，而且在这个过程也感受一下整个任务的运行逻辑。在后面写多个任务同步的时候，思路也会更加清晰，还可以体会到裸机代码的缺点，和跑操作系统的实时性。

## 分析Demo源码
这里提供了一个案例源码可以供大家参考一下，以下是对源码分析：

1. 头文件的分析
```c
/* 系统头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* 用户驱动头文件 */
#include "gpio.h"          // GPIO底层代码
#include "led.h"           // LED控制
#include "oled.h"          // OLED显示
#include "temperature.h"   // 温湿度传感器
#include "key.h"           // 按键检测
#include "dc_motor.h"      // 直流电机
#include "step_motor.h"    // 步进电机
```
系统头文件：生成自带的，包含了STM32库函数的版本，和FreeRTOS的API。
用户驱动头文件：用户自定义的API；有一些教程教用户用一个类似headfile.h的头文件，将所有的.h全部包含，但是这种方法我感觉并不是特别好，我的建议还是，用到哪一个头文件，再引用对应的即可。
2. 数据结构定义
```c
// 传感器消息结构体
typedef struct {
    float temperature;  // 温度值
    float humidity;     // 湿度值
} sensor_msg_t;
```
在不同的任务中，一般是使用队列传输信息，但是不可能传输一个消息就定义一个队列，这样太浪费资源了，所以会将一类的资源，放在一个结构体中，方便传输
3. 按键事件宏定义
```c
#define KEY1_PRESSED   (1U << 0)  // 按键1按下
#define KEY1_RELEASED  (1U << 3)  // 按键1释放
#define KEY2_PRESSED   (1U << 1)  // 按键2按下
#define KEY2_RELEASED  (1U << 4)  // 按键2释放
#define KEY3_PRESSED   (1U << 2)  // 按键3按下
#define KEY3_RELEASED  (1U << 5)  // 按键3释放
```
4. 全局变量定义
```c
// 传感器数据
float temperature = 0.0f;
float humidity = 0.0f;

// 任务句柄和属性
osThreadId_t key_task_handle;        // 按键任务
osThreadId_t dip_task_handle;        // 显示任务  
osThreadId_t env_collect_task_handle; // 环境采集任务
osThreadId_t control_task_handle;    // 控制任务

// 消息队列
osMessageQueueId_t sensor_queue;     // 传感器数据队列
```
5. 任务配置表

| 任务名称    | 堆栈大小 | 优先级  | 功能描述           |
|------------|----------|---------|--------------------|
| keyTask    | 512字节  | High    | 按键扫描和事件分发 |
| dipTask    | 512字节  | Normal  | OLED显示更新       |
| envCollectTask | 512字节 | Low     | 温湿度数据采集     |
| controlTask | 512字节 | Normal1 | 设备控制逻辑       |

## 任务详解
1. 按键扫描任务 (key_task)
周期性扫描3个按键状态（10ms周期），检测按键的按下和释放事件，使用位操作组合多个按键事件，通过线程标志同步通知其他任务
```c
void key_task(void *arg)
{
    uint8_t cnt = 0, key_value = KEY_NONE;
    gpio_config_t *keys[3] = {&key1, &key2, &key3};

    while(1)
    {
        uint32_t notify = 0;

        // 循环扫描3个按键
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

        // 如果有按键事件，通知其他任务
        if (notify) {
            osThreadFlagsSet(dip_task_handle, notify);    // 通知显示任务
            osThreadFlagsSet(control_task_handle, notify); // 通知控制任务
        }
        osDelay(10);  // 10ms扫描周期
    }
}
```

1. 显示任务 (dip_task)
```c
从消息队列获取传感器数据并显示，监听按键事件并更新按键状态显示，使用非阻塞方式检查事件和消息，'*'表示按键按下，'O'表示按键释放

void dip_task(void *arg)
{
    sensor_msg_t msg;
    uint32_t flags;

    while(1)
    {
        // 检查并更新温湿度显示
        if (osMessageQueueGet(sensor_queue, &msg, NULL, 0) == osOK) {
            oled_show_temperature(2, 7, msg.temperature, 12);
            oled_show_humidity(4, 7, msg.humidity, 12);
        }

        // 检查并更新按键状态显示
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

        osDelay(10);  // 10ms更新周期
    }
}
```
3. 环境采集任务 (env_collect_task)
```c
周期性采集温湿度数据（1秒周期），将数据封装成消息结构体，通过消息队列发送给显示任务，处理传感器读取错误情况

void env_collect_task(void *arg)
{
    sensor_msg_t msg;
    while(1)
    {
        // 读取温湿度传感器数据
        if (gxht3l_get_temp_humi(&temperature, &humidity) == GXHT3L_OK) {
            msg.temperature = temperature;
            msg.humidity = humidity;
            osMessageQueuePut(sensor_queue, &msg, 0U, 0U);
        }
        osDelay(1000);  // 1秒采集周期
    }
}
```
4. 控制任务
使用阻塞方式等待按键事件，根据不同的按键事件控制不同的外设，即事件驱动模式，无事件时不占用CPU资源，实现LED、步进电机、直流电机的控制
```c
void control_task(void *arg)
{
    while(1)
    {
        // 等待按键事件（阻塞方式）
        uint32_t flags = osThreadFlagsWait(
            KEY1_PRESSED | KEY1_RELEASED |
            KEY2_PRESSED | KEY2_RELEASED |
            KEY3_PRESSED | KEY3_RELEASED,
            osFlagsWaitAny, osWaitForever);

        // Key1: 控制LED
        if (flags & KEY1_PRESSED)  GPIO_Write_Low(&led1);   // 按下亮
        if (flags & KEY1_RELEASED) GPIO_Write_High(&led1);  // 释放灭

        // Key2: 控制步进电机
        if (flags & KEY2_PRESSED)  step_motor_run();        // 按下运行
        if (flags & KEY2_RELEASED) step_motor_stop();       // 释放停止

        // Key3: 控制直流电机
        if (flags & KEY3_PRESSED)  dc_motor_set_speed(499); // 按下运行
        if (flags & KEY3_RELEASED) dc_motor_stop();         // 释放停止
    }
}
```


## 通信机制分析
1. 线程标志 (Thread Flags)
线程通信是轻量级通信，适合传递事件标志，支持位操作，可以同时传递多个事件，可以选择阻塞或非阻塞方式接收
```c
// 发送线程标志
osThreadFlagsSet(dip_task_handle, notify);
// 接收线程标志（非阻塞）
flags = osThreadFlagsWait(ALL_KEY_EVENTS, osFlagsWaitAny, 0);
// 接收线程标志（阻塞）
flags = osThreadFlagsWait(ALL_KEY_EVENTS, osFlagsWaitAny, osWaitForever);
```
2. 消息队列 (Message Queue)
适合传递数据量较大的消息,自带缓冲区，支持多个消息排队,线程安全，支持多任务访问
```c
// 创建消息队列
sensor_queue = osMessageQueueNew(4, sizeof(sensor_msg_t), NULL);
// 发送消息
osMessageQueuePut(sensor_queue, &msg, 0U, 0U);
// 接收消息（非阻塞）
osMessageQueueGet(sensor_queue, &msg, NULL, 0);
```


