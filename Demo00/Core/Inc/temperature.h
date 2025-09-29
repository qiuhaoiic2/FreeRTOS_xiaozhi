#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stdint.h>
#include "i2c.h"

extern float temperature;
extern float humidity;

// 错误码定义
typedef enum {
    GXHT3L_OK = 0,
    GXHT3L_ERROR_I2C = 1,
    GXHT3L_ERROR_CRC = 2
} gxht3l_error_t;

// 原有函数
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);

// 新增函数
gxht3l_error_t gxht3l_init(void);
gxht3l_error_t gxht3l_get_temp_humi(float *temp, float *humi);

#endif /* TEMPERATURE_H */
