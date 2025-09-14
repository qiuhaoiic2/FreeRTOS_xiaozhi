# FreeRTOS 互斥量与优先级继承演示

## 项目概述

本项目演示了 FreeRTOS 中互斥量（Mutex）的使用以及优先级继承机制如何解决优先级反转问题。

## 功能说明

### 任务设计

项目创建了三个不同优先级的任务：

- **low_task (优先级1-最低)**：模拟低优先级任务持有共享资源
- **mid_task (优先级2-中等)**：模拟中等优先级任务持续运行，制造优先级反转场景  
- **high_task (优先级3-最高)**：模拟高优先级任务需要访问共享资源

### 演示场景

1. `low_task` 首先获取互斥量并持有3秒
2. `high_task` 在1.5秒后尝试获取互斥量，被阻塞
3. `mid_task` 持续运行，展示优先级反转问题
4. 通过互斥量的优先级继承机制解决反转问题

## 关键代码分析

### 互斥量创建
```c
// 注意：当前使用二值信号量，应改为互斥量以启用优先级继承
xMutex = xSemaphoreCreateBinary();  // 应改为 xSemaphoreCreateMutex()
```

### 优先级继承机制

当 `high_task` 等待被 `low_task` 持有的互斥量时：
- `low_task` 的优先级临时提升到与 `high_task` 相同
- 防止 `mid_task` 抢占 `low_task`
- `low_task` 释放锁后优先级恢复原值

## 已知问题与修复建议

### 1. 错误使用二值信号量
**问题**：当前使用 `xSemaphoreCreateBinary()`，不支持优先级继承
**修复**：
```c
xMutex = xSemaphoreCreateMutex();  // 使用互斥量
```




## 预期输出

正常情况下应该看到：
```
low_task: trying to get mutex...
low_task: start hold UART 3 s
mid_task: running...
high_task: wait mutex...
low_task: release UART
high_task: got lock!
```

## 编译与运行

1. 确保 FreeRTOS 配置正确
2. 编译项目
3. 下载到目标硬件
4. 观察串口输出验证优先级继承效果

## 学习要点

- 理解优先级反转问题的产生原因
- 掌握互斥量与二值信号量的区别
- 学会使用优先级继承解决实时性问题
- 熟悉 FreeRTOS 任务调度机制

## 参考资料

- FreeRTOS 官方文档
- 《FreeRTOS 实时操作系统》
- ARM Cortex-M 系列处理器参考手册
