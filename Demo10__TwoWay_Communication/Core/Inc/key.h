#ifndef _KEY_H
#define _KEY_H


#include "main.h"
#include "gpio.h"

typedef enum {
    KEY_NONE = 0,
    KEY_PRESS,
    KEY_RELEASE,
    KEY_SHORT,
    KEY_LONG
}key_event_t;

typedef struct {
    uint8_t last_level;     // 上一次电平
    uint8_t press_flag;     // 已按下标记
    uint32_t down_time;   // 按下时间
} key_state_t;



extern gpio_config_t key1;
extern gpio_config_t key2;
extern gpio_config_t key3;

key_event_t key_scan(gpio_config_t *key);



#endif
