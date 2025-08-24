# Demo07-CountingSemaphore  
**一键三灯齐闪：计数信号量 vs 二值信号量**

---

## 📌 目标
用 **计数信号量** 实现 **“一个按键 → 三个 LED 同时翻转”**；  
直观对比 **二值信号量只能唤醒 1 个任务** 的局限。

---

## 🧪 实验现象
| 场景 | 按键次数 | LED 响应 |
|---|---|---|
| 二值信号量 | 按 3 次 | **只有 1 个 LED** 依次闪 |
| 计数信号量 | 按 **1 次** | **3 个 LED 同时闪** |

---

## 🛠️ 使用步骤

| 步骤 | 函数 | 说明 |
|---|---|---|
| ① 创建 | `xSemaphoreCreateCounting(3,0)` | 最多 3 把钥匙 |
| ② 发送 | `xSemaphoreGive()` ×3 | 按键一次性给 3 把钥匙 |
| ③ 接收 | `xSemaphoreTake()` | 3 个 LED 任务同时拿到钥匙 |

---

## 📋 关键代码

```c
/* 创建计数信号量 */
Counting_Semaphore = xSemaphoreCreateCounting(3, 0);

/* 按键任务：一次性给出 3 把钥匙 */
for (uint8_t i = 0; i < 3; i++)
    xSemaphoreGive(Counting_Semaphore);

/* LED 任务：拿到钥匙就翻转 */
xSemaphoreTake(Counting_Semaphore, portMAX_DELAY);

## 计数信号量的缺点

1. 虽然可以存储多次信号量，但是还是存在一个问题，不能分辨出发送方到底是谁，也不能知道发送的消息有什么意义
