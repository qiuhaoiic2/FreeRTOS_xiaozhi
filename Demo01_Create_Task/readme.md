# 第一个demo，移植FreeRTOS到小智学长开发板

## 下载FreeRTOS源码
1. 进入网址 `https://github.com/FreeRTOS` 下载FreeRTOS源码
2. 进入网站其实可以看到有两个版本的FreeRTOS，一个是 Kernel（核心），一个是Classic（经典)
3. 这里下载 Kernel 核心代码版本的即可，版本建议选择跟我一样用10.4.3即可


### 补充说明
1. 为什么不使用cubemx生成rtos？答：生成其实就是在源码上面封装了一层，但是不容易看到底层源码，并且可移植性差
2. Classic版本是干嘛的？答：1、提供了一些芯片历程的demo，包括F103 keil版本的，但是使用标准库实现的
                           2、plus版本有vs studio 仿真的版本，即不需要开发板也能在电脑上调试rtos，感兴趣可以配置。

## cubemx 修改一下系统配置
- 由于FreeRTOS使用 ` SysTick ` 当作心跳，HAL库也使用了` SysTick `当作超时计数用`HAL_Delay()`
- FreeRTOS和HAL库不能同时使用SysTick定时器，所在在这里将HAL库的定时器改为其他的定时器，我改成了定时器2

## keil配置FreeRTOS

配置这里参考官方文档：`https://www.freertos.org/zh-cn-cmn-s/Documentation/02-Kernel/03-Supported-devices/01-FreeRTOS-porting-guide`

1. source 文件配置：添加文件
- ├── Source/tasks.c
- ├── Source/Queue.c
- ├── Source/List.c
- ├── Source/portable/RVDS/ARM_CM3/port.c
- ├── Source/portable/MemMang/heap_4.c
- 为什么选择 RVDS 文件夹，可以看一下keil文件的说明

2. include 文件配置：添加路径
- ├── Source/include
- ├── Source/portable/RVDS/ARM_CM3/

## 配置 FreeRTOSConfig.h 文件
配置这里参考官方文档：`https://www.freertos.org/zh-cn-cmn-s/Documentation/02-Kernel/03-Supported-devices/02-Customization`

1. 手动配置`FreeRTOSConfig.h`文件，这个文件需要我们自己手动新建一个
2. 根据自己的要求去裁剪或者添加宏定义即可，


3. 相关宏大致可以分为三类。
- **‘INCLUDE’开头**  ---  配置FreeRTOS中可选的API函数。
- **’config‘开头**   ---  完成FreeRTOS的功能配置和裁剪(如调度方式、使能信号量功能等)。
- **其他配置**       ---  PendSV宏定义、SVC宏定义、SysTick_Handler

## 库函数的修改
- #define __NVIC_PRIO_BITS          4U（需要去掉这个U，要不然汇编会报错）
