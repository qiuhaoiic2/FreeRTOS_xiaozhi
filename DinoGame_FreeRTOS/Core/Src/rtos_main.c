#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "spi.h"
#include "key.h"
#include "stdlib.h"
#include "tim3.h"
static void log_task(void *para);
static void key_task(void *para);
static void gameover_task(void *para);
static void display_task(void *para);

uint16_t Grade = 0; /* 分数 */
uint8_t Speed = 2;  /* 速度 */

// Cloud
const uint8_t Cloud_Length = 27;

// 恐龙数据结构
typedef struct
{
    uint8_t height;     // 0-25
    uint8_t frame;      // 跑步动画 0/1
    uint8_t jump_phase; // 0-15 跳跃阶段，0=未跳跃
} dino_state_t;
static const uint8_t kJumpTable[16] = {
    0, 6, 10, 15, 18, 21, 23, 25, 25,23, 21, 18, 15, 10, 6};
QueueHandle_t xDinoEvent; // 1 字节事件：按键是否按下
QueueHandle_t xDinoState; // 4 字节状态

// 任务句柄
TaskHandle_t display_task_handle = NULL;
TaskHandle_t gameover_task_handle = NULL;

typedef struct
{
    uint8_t id;    // 0/1/2 三种仙人掌
    int8_t x;      // 初始 127
    uint8_t speed; // 当前全局速度
} cactus_evt_t;
static const uint8_t kCactusLen[3] = {8, 16, 16}; // 对应 Cactus_Length1/2/3
static uint16_t cactus_ticks = 0;
static uint16_t cactus_next = 60;             // 首次 60*20ms = 1.2 s
static uint16_t cactus_ticks_multiplier = 60; // (1~3)*60*20ms = 1.2 s

typedef struct
{
    int16_t x; // 当前像素坐标
    uint8_t speed;
    uint8_t active; // 1=在屏，0=已回收
} cactus_t;
QueueHandle_t xCactusEvt; // 4 字节状态
static uint8_t check_collision(dino_state_t dino, cactus_t cactus, uint8_t cactus_id);

void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);
    // 创建队列和互斥锁
    xCactusEvt = xQueueCreate(5, sizeof(cactus_evt_t));
    xDinoEvent = xQueueCreate(2, sizeof(uint8_t));
    xDinoState = xQueueCreate(1, sizeof(dino_state_t));

    configASSERT(xDinoEvent && xDinoState && xCactusEvt);

    oled_bind_spi(&oled, &user_spi3);
    oled.oled_initialize(&oled);
    oled.oled_clear_screen(&oled);

    Speed = 2;
    Grade = 0;

    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);
    xTaskCreate(display_task, "Disp", 256, &oled, 2, &display_task_handle);
    xTaskCreate(gameover_task, "gameover", 256, &oled, 4, &gameover_task_handle);
    xTaskCreate(key_task, "Key", 128, NULL, 3, NULL);

    vTaskStartScheduler();
    while (1) // 如果成功创建了任务，则不会跑到这里来
    {
        ;
        ;
    }
}
static void display_task(void *para)
{
    oled_config_t *oled = (oled_config_t *)para;
    uint16_t Ground_Move_Number = 0;
    int8_t Cloud_Positon[2] = {100, 0};
    dino_state_t dino_information;
    static uint8_t last_dino_y = 5; // 记录上一帧恐龙的Y坐标

    static cactus_t cactus_information[3] = {0}; // 仙人掌相关信息
    cactus_evt_t new_cactus;

    uint8_t game_over = 0;

    oled->OLED_ShowGameBegin(oled, 2, 0, 129, 7);
    log_print("Game Start!");
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    log_print("Game Running!");
    oled->oled_clear_screen(oled);

    while (1)
    {
        if (xQueueReceive(xCactusEvt, &new_cactus, 0) == pdTRUE)
        {
            cactus_information[new_cactus.id].x = new_cactus.x;
            cactus_information[new_cactus.id].speed = new_cactus.speed;
            cactus_information[new_cactus.id].active = 1;
            log_print("New cactus %d", new_cactus.id);
        }
        if (xQueueReceive(xDinoState, &dino_information, 0) == pdTRUE)
        {
            log_print("dino height = %d", dino_information.height);
        }

        // 分数显示
        oled->OLED_ShowNum(oled, 1, 12, Grade, 5);
        // 地面
        Ground_Move_Number += Speed;
        Ground_Move_Number %= 560;
        oled->OLED_ShowGroud(oled, 0, 6, 127, 7, Ground_Move_Number);

        // 云朵显示
        for (uint8_t i = 0; i < 2; i++)
        {
            oled->OLED_ClearPicture(oled, Cloud_Positon[i] + 1, 2 + (i == 0), Cloud_Positon[i] + Cloud_Length - 1, 2 + (i == 0));
            Cloud_Positon[i] -= (Speed > 1 ? Speed - 1 : 1);
            if (Cloud_Positon[i] < -27)
                Cloud_Positon[i] = 127;
            oled->OLED_ShowCloud(oled, Cloud_Positon[i], 2 + (i == 0), Cloud_Positon[i] + Cloud_Length - 1, 2 + (i == 0));
        }

        // 恐龙显示
        /* 计算显示坐标 */
        uint8_t dino_y;
        if (dino_information.height <= 2)
            dino_y = 5;          // 地面
        else if (dino_information.height <= 8)
            dino_y = 4;          // 低跳
        else if (dino_information.height <= 15)
            dino_y = 3;          // 中跳
        else if (dino_information.height <= 20)
            dino_y = 2;          // 高跳
        else
            dino_y = 1;          // 最高跳
        // 清除上一帧和当前帧可能的位置，避免残影
        uint8_t clear_y_start = (last_dino_y < dino_y) ? last_dino_y : dino_y;
        uint8_t clear_y_end = (last_dino_y > dino_y) ? last_dino_y + 1 : dino_y + 1;
        if (clear_y_end > 6) clear_y_end = 6; // 不超过地面
        
        oled->OLED_ClearPicture(oled, 0, clear_y_start, 15, clear_y_end);
        
        // 显示恐龙
        // if (dino_information.height == 0)
        //     oled->OLED_ShowDino(oled, 0, dino_y, 15, dino_y + 1, dino_information.frame);
        // else
        //     oled->OLED_ShowDino_Jump(oled, 0, dino_y, 15, dino_y + 1, dino_information.frame);
        
        // 记录当前帧位置为下一帧的上一帧
        last_dino_y = dino_y;

        oled->OLED_ShowDino(oled, 0, dino_y, 15, dino_y + 1, dino_information.frame);

        // 仙人掌显示
        for (uint8_t id = 0; id < 3; id++)
        {
            if (!cactus_information[id].active)
                continue;

            /* 清旧像素 */
            int16_t len = kCactusLen[id];
            oled->OLED_ClearPicture(oled,
                                    cactus_information[id].x + 1,
                                    5 + (id == 2), // 仙人掌 2 放在 y=6
                                    cactus_information[id].x + len - 1,
                                    6);
            /* 左移 */
            cactus_information[id].x -= cactus_information[id].speed;

            /* 出屏回收 */
            if (cactus_information[id].x < -len)
            {
                cactus_information[id].active = 0;
                continue;
            }
            /* 碰撞检测 */
            if (check_collision(dino_information, cactus_information[id], id) == 1)
            {
                game_over = 1;
                xTaskNotifyGive(gameover_task_handle);
                HAL_TIM_Base_Stop_IT(&htim3);
                break;
            }

            /* 画新像素 */
            switch (id)
            {
            case 0:
                oled->OLED_ShowCactus1(oled, cactus_information[id].x, 5, cactus_information[id].x + len - 1, 6);
                break;
            case 1:
                oled->OLED_ShowCactus2(oled, cactus_information[id].x, 5, cactus_information[id].x + len - 1, 6);
                break;
            case 2:
                oled->OLED_ShowCactus3(oled, cactus_information[id].x, 6, cactus_information[id].x + len - 1, 6);
                break;
            }
        }
        if (game_over)
        {
            log_print("Game Over!");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

static void key_task(void *para)
{
    key_event_t key_value = KEY_NONE;
    uint8_t cnt = 0;
    static uint8_t game_begin_flag = false;
    gpio_config_t *keys[3] = {&key1, &key2, &key3};

    for (cnt = 0; cnt < 3; cnt++)
    {
        GPIO_Init_Universal(keys[cnt]);
    }

    while (1)
    {
        for (cnt = 0; cnt < 3; cnt++)
        {
            key_value = key_scan(keys[cnt]);
            if (key_value == KEY_SHORT)
            {
                if (game_begin_flag == false)
                {
                    game_begin_flag = true;
                    xTaskNotifyGive(display_task_handle);
                    HAL_TIM_Base_Start_IT(&htim3);
                }
                else
                {
                    // 通知恐龙起跳
                    uint8_t jump_event = 1;
                    xQueueSendToBack(xDinoEvent, &jump_event, 0);
                }
                log_print("key %d short press", cnt + 1);
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/*
 *函数功能：STM32软复位函数

 */
static void Stm32_SoftReset(void)
{
    __set_FAULTMASK(1); // 禁止所有的可屏蔽中断
    NVIC_SystemReset(); // 软件复位
}

static void gameover_task(void *para)
{
    oled_config_t *oled = (oled_config_t *)para;
    uint8_t game_over = 0;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    oled->oled_clear_screen(oled);
    oled->OLED_ShowGameOver(oled, 0, -1, 127, 6);
    oled->OLED_ShowNum(oled, 4, 6, Grade, 6);
    while (1) // 游戏结束
    {
        if (xQueueReceive(xDinoEvent, &game_over, portMAX_DELAY) == pdPASS)
        {
            Stm32_SoftReset();
        }
    }
}
static void log_task(void *para)
{
    char message[LOG_MAX_LEN];
    while (1)
    {
        if (xQueueReceive(xLogQueue, message, portMAX_DELAY) == pdPASS)
        {
            printf("%s\r\n", message);
        }
    }
}
static uint32_t sys_tim = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        HAL_IncTick();
    }
    if (htim->Instance == TIM3)
    {
        BaseType_t xHigherPriorityTaskWoken, xHigherPriorityTaskWoken2;
        static dino_state_t dino = {0, 0, 0};

        sys_tim++;
        if (sys_tim % 50 == 0) //
        {
            // 处理小恐龙的奔跑跳跃
            uint8_t jump_event = 0;
            if (xQueueReceiveFromISR(xDinoEvent, &jump_event, &xHigherPriorityTaskWoken) == pdTRUE) // 非阻塞读取
            {
                if (jump_event == 1 && dino.jump_phase == 0) // 只能在地面起跳
                    dino.jump_phase = 1;
            }
            dino.height = kJumpTable[dino.jump_phase];

            dino.frame ^= 1;
            if (dino.jump_phase)
            {
                dino.jump_phase = dino.jump_phase > 15 ? 0 : dino.jump_phase + 1;
            }
            xQueueSendToBackFromISR(xDinoState, &dino, &xHigherPriorityTaskWoken);
        }
        // 生成新的仙人掌
        if (sys_tim % 20 == 0)
        {
            if (++cactus_ticks >= cactus_next)
            {
                uint8_t id = rand() % 3;
                cactus_evt_t evt = {id, 127, Speed};

                xQueueSendFromISR(xCactusEvt, &evt, &xHigherPriorityTaskWoken2);

                /* 计算下一次间隔：1~3 * 系数 */
                cactus_next = (rand() % 3 + 1) * cactus_ticks_multiplier;
                cactus_ticks = 0;
            }
        }
        // 加速逻辑
        if (sys_tim % 200 == 0)
        {
            Grade++;       
            // 渐进式加速，每10分增加0.2的速度（用整数模拟小数）
            static uint16_t speed_accumulator = 20; // 初始速度2.0 * 10
            
            if (Grade % 10 == 0) // 每10分加速一次
            {
                speed_accumulator += 2; // 增加0.2 * 10
                Speed = speed_accumulator / 10; // 转换为整数速度
                
                // 限制最大速度
                if (Speed > 8)
                    Speed = 8;
                
                log_print("Speed up to %d", Speed);
            }
            
            // 动态调整仙人掌生成频率
            if (Grade >= 50 && Grade < 100)
                cactus_ticks_multiplier = 55; // 稍微增加频率
            else if (Grade >= 100 && Grade < 150)
                cactus_ticks_multiplier = 50; // 进一步增加
            else if (Grade >= 150)
                cactus_ticks_multiplier = 45; // 最高频率
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken2);
    }
}
// 碰撞检测函数
static uint8_t check_collision(dino_state_t dino, cactus_t cactus, uint8_t cactus_id)
{
    int16_t cactus_right = cactus.x + kCactusLen[cactus_id] - 1;

    // 根据不同仙人掌类型使用原始高度值进行检测
    switch (cactus_id)
    {
    case 0: // 仙人掌1 (Cactus1)
        // 右边界在0-24范围内且恐龙高度<=14
        if (cactus_right <= 24 && cactus_right >= 0 && dino.height <= 14)
        {
            return 1; // 碰撞
        }
        break;

    case 1: // 仙人掌2 (Cactus2)
        // 右边界在0-26范围内且恐龙高度<=14
        if (cactus_right <= 26 && cactus_right >= 0 && dino.height <= 14)
        {
            return 1; // 碰撞
        }
        break;

    case 2: // 仙人掌3 (Cactus3)
        // 右边界在0-26范围内且恐龙高度<=6
        if (cactus_right <= 26 && cactus_right >= 0 && dino.height <= 6)
        {
            return 1; // 碰撞
        }
        break;
    }

    return 0; // 无碰撞
}
