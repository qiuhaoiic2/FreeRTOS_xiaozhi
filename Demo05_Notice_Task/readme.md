# 第四个demo，until和delay的区别

- 可以明显观察到LED1是900ms的闪烁频率，LED2是500ms的闪烁频率



## 浅谈一下Systick
- FreeRTOS的最小时间片，是一个Systick，如果Systick为1000hz，那么最小的tick就是1ms
- 在代码中计算的所有运行时间都是tick，而不是ms，所以需要一些转换函数
- 转换函数`pdMS_TO_TICKS();`是将tick转换为ms单位 
- 转换函数`TICKS_TO_MS();`是将ms转换为tick单位

## 计算代码运行了多少tick
- 使用函数xTaskGetTickCount();获取运行的tick时间
```c
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount ();
    code();// dosomethings

    xLastWakeTime = xTaskGetTickCount () - xLastWakeTime;
```

## delay 和 until的区别

1. 假设有10个tick，现在两个任务分别用的`vTaskDelay(2);` ` vTaskDelayUntil(2,2);`假色两个任务执行结束的tick都是tick2

2. 那么在vTaskDelay(2);这个任务中，2个tick已经过去了，任务变成了就绪态，但是因为优先级的问题 \\
   或者其他任务正在执行导致CPU被抢占，实际执行该任务就有可能会变成tick6，或者tick10。
   
3. 而vTaskDelayUntil(2,2);这个任务中，会记录tick2时刻，并且在tick4时刻一定得到执行


