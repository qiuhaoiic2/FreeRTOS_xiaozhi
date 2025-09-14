# 第二个demo，使用静态的方式创建任务


## 首先修改FreeRTOSConfig文件中开启静态创建的方法

- `configSUPPORT_STATIC_ALLOCATION`将该宏定义定义为1

## 补齐静态创建所需的函数
- 在网站`https://www.freertos.org/zh-cn-cmn-s/Documentation/02-Kernel/03-Supported-devices/02-Customization#configsupport_static_allocation`
- Ctrl + F 搜索 `configSUPPORT_STATIC_ALLOCATION`,即可看到相关说明，我们把提供的函数直接复制过来即可

## 静态函数创建所需参数
- 直接参考官网的示例填写即可 `https://www.freertos.org/zh-cn-cmn-s/Documentation/02-Kernel/04-API-references/01-Task-creation/02-xTaskCreateStatic`

## 静态创建使用场景

- 静态创建可将任务堆栈放置在特定的内存位置，不会出现内存分配失败的情况
- 动态创建使用起来相对简单。在实际的应用中，动态方式创建任务是比较常用的，除非有特殊的需求，一般都会使用动态方式创建任务 。


