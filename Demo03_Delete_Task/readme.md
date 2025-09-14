# LED 控制任务说明

这个项目包含三个任务：`led1_task`、`led2_task` 和 `log_task`，用于控制两个LED灯的闪烁，并记录日志信息。以下是详细的说明：


## 功能说明

### LED1 控制任务 (`led1_task`)

- 初始化并配置LED1的GPIO。
- 将LED1设置为高电平。
- 进入无限循环，每500毫秒切换一次LED1的状态。
- 当计数器达到4时，打印日志信息并挂起当前任务 (`vTaskSuspend(NULL)`)。

### LED2 控制任务 (`led2_task`)

- 初始化并配置LED2的GPIO。
- 将LED2设置为高电平。
- 进入无限循环，每500毫秒切换一次LED2的状态。
- 当计数器达到8时，打印日志信息，恢复LED1任务 (`vTaskResume(Led1_Handle)`)，并退出任务 (`vTaskDelete(NULL)`)。

### 日志记录任务 (`log_task`)

- 从日志队列中接收消息，并打印到控制台。

