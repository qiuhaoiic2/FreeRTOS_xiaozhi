#include "key.h"
#include "gpio.h"
gpio_config_t key1 = {
    .GpioPort = GPIOC,
    .Pin      = GPIO_PIN_3,
    .Mode     = GPIO_MODE_INPUT,
    .Pull     = GPIO_PULLUP,
    .Speed    = GPIO_SPEED_FREQ_LOW,
    .value    = 1
};
gpio_config_t key2 = {
    .GpioPort = GPIOC,
    .Pin      = GPIO_PIN_2,
    .Mode     = GPIO_MODE_INPUT,
    .Pull     = GPIO_PULLUP,
    .Speed    = GPIO_SPEED_FREQ_LOW,
    .value    = 1
};
gpio_config_t key3 = {
    .GpioPort = GPIOC,
    .Pin      = GPIO_PIN_1,
    .Mode     = GPIO_MODE_INPUT,
    .Pull     = GPIO_PULLUP,
    .Speed    = GPIO_SPEED_FREQ_LOW,
    .value    = 1
};



static key_state_t key1_state = {0};
static key_state_t key2_state = {0};
static key_state_t key3_state = {0};

//绑定key
static key_state_t *get_state(gpio_config_t *key)
{
    if (key == &key1) return &key1_state;
    if (key == &key2) return &key2_state;
    if (key == &key3) return &key3_state;
    return NULL;
}

key_event_t key_scan(gpio_config_t *key)
{
    key_state_t *key_s = NULL;
    key_s = get_state(key);
    if (key_s == NULL)  return KEY_NONE;

    if (GPIO_Read_Pin(key) == -1)
        return KEY_NONE;

    if (key->value == 0)
    {
        key_s->last_level = 1;
        key_s->down_time = HAL_GetTick();
    }
    if (key->value == 1 && key_s->last_level == 1)
    {
        key_s->last_level = 0;
        key_s->down_time = HAL_GetTick() - key_s->down_time;
        if (key_s->down_time <= 2000)
            return KEY_SHORT;
        else
            return KEY_LONG;
    }
    return KEY_NONE;
}
/**
 * @brief 检测按键是否按下（简化版）
 * @param key 按键配置指针
 * @retval 1: 按键按下, 0: 按键未按下, -1: 错误
 */
key_event_t key_is_pressed(gpio_config_t *key)
{
    if (GPIO_Read_Pin(key) == -1)
        return KEY_NONE;  // 读取失败

    // 由于按键使用上拉电阻，按下时为0，未按下为1
    return (key->value == 0) ? KEY_PRESS : KEY_RELEASE;
}
