#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "main.h"

extern uint32_t SystemCoreClock;

/* Here is a good place to include header files that are required across
  your application. */

#define configUSE_PREEMPTION                                        1 //抢占优先级
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                     1 //硬件抢占方法
#define configUSE_TICKLESS_IDLE                                     0
#define configCPU_CLOCK_HZ                                          SystemCoreClock
#define configSYSTICK_CLOCK_HZ                                      (SystemCoreClock / 8)
#define configTICK_RATE_HZ                                          1000 //时间片为1ms 
#define configMAX_PRIORITIES                                        32   //最大优先级
#define configMINIMAL_STACK_SIZE                                    128  //空闲任务堆栈大小，单位是：word（4字节）
#define configMAX_TASK_NAME_LEN                                     16   // 定义名字的最大值
#define configUSE_16_BIT_TICKS                                      0    // 8位 或者 16位的单片机定义为1，可提升性能
#define configIDLE_SHOULD_YIELD                                     1    // 任务可以抢占空闲任务
#define configUSE_TASK_NOTIFICATIONS                                1    // 任务通知相关的api
#define configTASK_NOTIFICATION_ARRAY_ENTRIES                       1    // 任务通知数组的索引(10.4.0之前索引只有一个)
#define configUSE_MUTEXES                                           0    // 互斥锁功能
#define configUSE_RECURSIVE_MUTEXES                                 0    // 递归互斥锁功能
#define configUSE_COUNTING_SEMAPHORES                               0    // 计数信号量

#define configQUEUE_REGISTRY_SIZE                                   10   // 定义可以注册的队列和信号量的最大数量
#define configUSE_QUEUE_SETS                                        0    // 队列集
#define configUSE_TIME_SLICING                                      1    // 相同优先级的任务使用时间片调度
#define configUSE_NEWLIB_REENTRANT                                  0    // Newlib支持
#define configENABLE_BACKWARD_COMPATIBILITY                         0    // 8.0.0之前版本的库是否编译
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS                     0    // 线程本地存储数组的索引数
#define configUSE_MINI_LIST_ITEM                                    1    // 使能 MiniListItem_t，默认为1，如果定义为0，ListItem_t = MiniListItem_t
#define configSTACK_DEPTH_TYPE                                      uint16_t  //栈深度
#define configMESSAGE_BUFFER_LENGTH_TYPE                            size_t    //消息缓冲区变量类型
#define configHEAP_CLEAR_MEMORY_ON_FREE                             0   //使用pvPortMalloc()分配的内存块，会通过vPortfree释放，默认0

/* Memory allocation related definitions. */


#define configSUPPORT_STATIC_ALLOCATION                             1   //静态申请内存，如果为1使能静态创建，并且需要实现两个f函数
#define configSUPPORT_DYNAMIC_ALLOCATION                            1   //动态申请内存
#define configTOTAL_HEAP_SIZE                                       ((size_t)(10*1024)) //堆栈申请10kb的内存，最小单位为byte字节
#define configAPPLICATION_ALLOCATED_HEAP                            0   //用户手动分配堆在内存中的位置，默认为0，由freertos自动分配
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP                   0   //用户手动实现创建任务的堆栈分配、释放函数，默认为0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                                 0     //空闲任务钩子
#define configUSE_TICK_HOOK                                 0     //滴答任务钩子
#define configCHECK_FOR_STACK_OVERFLOW                      0     //堆栈溢出检测
#define configUSE_MALLOC_FAILED_HOOK                        0     //malloc申请失败 回调函数函数原型是void vApplicationMallocFailedHook( void );
#define configUSE_DAEMON_TASK_STARTUP_HOOK                  0     //如果和configUSE_TIMERS都设置为1，那么程序必须定义一个初始化钩子函数void void vApplicationDaemonTaskStartupHook( void );
#define configUSE_SB_COMPLETED_CALLBACK                     0     //数据流缓冲区

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                       0     //定时器统计程序运行时间（需要自己实现定时器，中断）
#define configUSE_TRACE_FACILITY                            0     //使能可视化追踪调试，默认0
#define configUSE_STATS_FORMATTING_FUNCTIONS                0     //如果和 configUSE_TRACE_FACILITY都为1的时候，会使能函数vTaskList()和vTaskGetRunTimeStats();

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                               0     //启用协程调度相关的api，是FreeRTOS的简化版任务
#define configMAX_CO_ROUTINE_PRIORITIES                     1     //协程最大优先级

/* Software timer related definitions. */
#define configUSE_TIMERS                                    0     //使能软件定时器相关的api
#define configTIMER_TASK_PRIORITY                           3     //软件定时器的优先级
#define configTIMER_QUEUE_LENGTH                            10    //软件定时器
#define configTIMER_TASK_STACK_DEPTH                        configMINIMAL_STACK_SIZE   //堆栈大小

/* Interrupt nesting behaviour configuration. */
#ifdef __NVIC_PRIO_BITS
   #define configPRIO_BITS __NVIC_PRIO_BITS
#else
   #define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15                  /* 中断最低优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5                   /* FreeRTOS可管理的最高中断优先级 */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))//stm32 用的是高4位的寄存器，故需要往左移动4位，我们平时配置nvic的时候，系统会自动帮我们配置好该寄存器。
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_API_CALL_INTERRUPT_PRIORITY           configMAX_SYSCALL_INTERRUPT_PRIORITY          //是configMAX_SYSCALL_INTERRUPT_PRIORITY新名称，仅用与版本移植
/* Define to trap errors during development. */
// #define vAssertCalled(char, int) printf("Error: %s, %d\r\n", char, int)
// #define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )






/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                1     //设置任务优先级
#define INCLUDE_uxTaskPriorityGet               1     //获取任务优先级
#define INCLUDE_vTaskDelete                     1     //删除任务
#define INCLUDE_vTaskSuspend                    1     //挂起任务
#define INCLUDE_vTaskDelayUntil                 1     //任务绝对延时
#define INCLUDE_vTaskDelay                      1     //任务延时
#define INCLUDE_xTaskGetSchedulerState          1     //获取调度器的状态
#define INCLUDE_xTaskGetCurrentTaskHandle       1     //获取当前任务的句柄
#define INCLUDE_uxTaskGetStackHighWaterMark     0     //获取任务堆栈历史剩余最小值
#define INCLUDE_uxTaskGetStackHighWaterMark2    0     //获取任务堆栈历史剩余最小值版本2
#define INCLUDE_xTaskGetIdleTaskHandle          0     //获取空闲任务的句柄
#define INCLUDE_eTaskGetState                   0     //获取任务当前的状态
#define INCLUDE_xTimerPendFunctionCall          0     //函数的执行挂到定时器服务任务中
#define INCLUDE_xTaskAbortDelay                 0     //中断延时
#define INCLUDE_xTaskGetHandle                  0     //通过任务名字获取任务句柄
#define INCLUDE_xTaskResumeFromISR              1     //在中断中恢复挂起的任务

/* A header file that defines trace macro can be included here. */



 #define vPortSVCHandler     SVC_Handler
 #define xPortPendSVHandler  PendSV_Handler
 #define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */



