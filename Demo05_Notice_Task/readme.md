# Demo05_Notice_Task - FreeRTOS 任务通知示例

## 项目简介
本示例演示了 FreeRTOS 中任务通知（Task Notification）的使用方法。任务通知是一种轻量级的任务间通信机制，可以替代信号量、事件标志组等同步原语，具有更高的执行效率和更少的内存开销。

## 主要功能
- **任务通知机制**：使用 `xTaskNotifyGive()` 和 `ulTaskNotifyTake()` 实现任务间通信
- **按键控制**：通过按键触发任务通知
- **LED 控制**：LED 任务接收通知后切换状态
- **日志系统**：记录任务执行状态

## 系统架构

### 任务结构
1. **Led_Task**（3个实例）
   - 优先级：2
   - 堆栈大小：128 字节
   - 功能：等待任务通知，接收到通知后切换对应 LED 状态

2. **Key_Task**（3个实例）  
   - 优先级：3
   - 堆栈大小：128 字节
   - 功能：扫描按键，检测到短按后发送任务通知给对应的 LED 任务

3. **log_task**
   - 优先级：1
   - 堆栈大小：256 字节
   - 功能：处理日志消息输出

### 通信机制
- **任务通知**：Key_Task 通过 [`xTaskNotifyGive()`](Demo05_Notice_Task/Core/Src/rtos_main.c) 向对应的 LED 任务发送通知
- **日志队列**：使用 [`xLogQueue`](Demo05_Notice_Task/Core/Src/log.c) 进行日志消息传递

## 核心 API 使用

### 任务通知 API
```c
// 发送通知（递增通知值）
xTaskNotifyGive(TaskHandle_t xTaskToNotify);

// 等待并获取通知（清零通知值）
ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait);
```

### 关键代码片段

#### LED 任务实现
```c
static void Led_Task(void *para)
{
    gpio_config_t *led = (gpio_config_t *)para;
    GPIO_Init_Universal(led);
    GPIO_Write_High(led);
    
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // 等待通知
        log_print("%s get notice", pcTaskGetName(NULL));
        GPIO_Toggle_Pin(led);  // 切换 LED 状态
    }
}
```

#### 按键任务实现
```c
static void Key_Task(void *para)
{
    gpio_config_t *key = (gpio_config_t *)para;
    key_event_t key_value = KEY_NONE;
    GPIO_Init_Universal(key);
    
    while (1)
    {
        key_value = key_scan(key);
        if (key_value == KEY_SHORT)
        {
            // 根据按键发送对应的任务通知
            if (key == &key1) xTaskNotifyGive(Led1_Handle);
            if (key == &key2) xTaskNotifyGive(Led2_Handle);
            if (key == &key3) xTaskNotifyGive(Led3_Handle);
            log_print("%s give notice", pcTaskGetName(NULL));
        } 
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

## 工作流程
1. 系统启动后创建 3 个 LED 任务和 3 个按键任务
2. LED 任务进入阻塞状态，等待任务通知
3. 按键任务持续扫描按键状态
4. 当检测到按键短按时，向对应的 LED 任务发送通知
5. LED 任务收到通知后，切换 LED 状态并输出日志
6. 循环执行上述过程

## 硬件要求
- STM32F1xx 系列微控制器
- 3 个 LED（led1, led2, led3）
- 3 个按键（key1, key2, key3）
- UART 用于日志输出

## 任务通知的优势
1. **高效性**：比队列、信号量等机制更快
2. **低内存占用**：不需要额外的内核对象
3. **简单性**：API 简洁易用
4. **灵活性**：支持多种通知模式（计数、位操作等）

## 编译和运行
1. 使用 STM32CubeIDE 或 Keil MDK 打开项目
2. 编译并下载到目标硬件
3. 通过串口查看日志输出
4. 按下按键观察对应 LED 的状态变化

## 注意事项
- 任务通知是单向的，只能从一个任务发送到另一个任务
- 每个任务只有一个通知值，如果需要多个通知源，需要合理设计通知机制
- 本示例使用了计数型通知，适合简单的同
