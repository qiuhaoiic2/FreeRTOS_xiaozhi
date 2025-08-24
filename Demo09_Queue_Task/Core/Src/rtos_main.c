#include "rtos_main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h"
#include "key.h"
static void log_task(void *para);
static void display_digit(uint8_t num);   // num = 0~7
static void Led_Task(void *para);
static void Key_Task(void *para);


typedef enum {
    SUB,
    ADD
}Event_t;

typedef struct {
    char *name;
    Event_t flag;
}Queue_Data_t;

QueueHandle_t Queue_Date = NULL;


void Start_Task()
{
    log_init();
    configASSERT(xLogQueue != NULL);

    Queue_Date = xQueueCreate(2,sizeof(Queue_Data_t));
    xTaskCreate(log_task, "log_task", 256, NULL, 1, NULL);

    xTaskCreate(Led_Task, "Led_Task", 128, NULL, 2, NULL);
    xTaskCreate(Key_Task, "Key1_Task", 128, &key1, 3, NULL);
    xTaskCreate(Key_Task, "Key2_Task", 128, &key2, 3, NULL);

    vTaskStartScheduler();
    while (1) //如果成功创建了任务，则不会跑到这里来
    {
        ;
        ;
    }
}
static void Led_Task(void *para)
{
    Queue_Data_t led_queue = {
        .flag = ADD,
        .name = pcTaskGetName(NULL)
    };
    uint8_t led_status = 0;
    GPIO_Init_Universal(&led1);
    GPIO_Init_Universal(&led2);
    GPIO_Init_Universal(&led3);
    display_digit(led_status);
    while (1)
    {
        xQueueReceive(Queue_Date,&led_queue,portMAX_DELAY);
        if (led_queue.flag == ADD && led_status < 7)
        {
            led_status++;
        }
        else if ( led_status > 0 && led_queue.flag == SUB)
        {
            led_status--;
        }
        display_digit(led_status);
        log_print("LED now: %d ", led_status);

    }
}


static void Key_Task(void *para)
{
    gpio_config_t *key = (gpio_config_t *)para;
    key_event_t key_value = KEY_NONE;
    Queue_Data_t key_queue = {
        .flag = ADD,
        .name = pcTaskGetName(NULL)
    };
    GPIO_Init_Universal(key);
    while (1)
    {
        key_value = key_scan(key);
        if (key_value == KEY_SHORT)
        {
            if (key == &key1)
            {
                key_queue.flag = ADD;
            }
            else if (key == &key2)
            {
                key_queue.flag = SUB;
            }
            xQueueSendToBack(Queue_Date,&key_queue,0);
            log_print("%s short press and key_queue flag is %s", key_queue.name, key_queue.flag == ADD ? "ADD" : "SUB");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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

static void display_digit(uint8_t num)   // num = 0~7
{
    /* 先全部熄灭 */
    GPIO_Write_High(&led1);
    GPIO_Write_High(&led2);
    GPIO_Write_High(&led3);

    /* 按位点亮 */
    if (num & 0x01) GPIO_Write_Low(&led1);   // bit0 → LED1
    if (num & 0x02) GPIO_Write_Low(&led2);   // bit1 → LED2
    if (num & 0x04) GPIO_Write_Low(&led3);   // bit2 → LED3

}
