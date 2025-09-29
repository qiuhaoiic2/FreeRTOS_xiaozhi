#include "temperature.h"
#include <stdio.h>

#define CRC8_POLYNOMIAL 0x31



/**
 * @brief CRC8校验函数
 * @param message 待校验的数据
 * @param initial_value 初始值
 * @retval CRC8校验值
 */
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t remainder;
    uint8_t i = 0, j = 0;

    remainder = initial_value;
    for(j = 0; j < 2; j++)
    {
        remainder ^= message[j];
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

/**
 * @brief 将原始数据转换为温湿度值
 * @param dat 原始数据数组
 * @param temperature 温度指针
 * @param humidity 湿度指针
 * @retval 0: 成功, 1: CRC校验失败
 */
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;

    /* 校验温度数据和湿度数据是否接收正确 */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;

    /* 转换温度数据 */
    recv_temperature = ((uint16_t)dat[0] << 8) | dat[1];
    *temperature = -45 + 175 * ((float)recv_temperature / 65535);

    /* 转换湿度数据 */
    recv_humidity = ((uint16_t)dat[3] << 8) | dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);

    return 0;
}
/**
 * @brief 初始化GXHT3L传感器（启动高精度测量）
 * @retval gxht3l_error_t 错误码
 */
gxht3l_error_t gxht3l_init(void)
{
    // 启动高精度测量
    uint8_t start_cmd[2] = {0x21, 0x30};
    if (HAL_I2C_Master_Transmit(&hi2c2, 0x88, start_cmd, 2, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    // 等待测量完成（高精度需要15ms）
    HAL_Delay(15);
    
    return GXHT3L_OK;
}
/**
 * @brief 获取温湿度数据
 * @param temp 温度指针
 * @param humi 湿度指针
 * @retval gxht3l_error_t 错误码
 */
gxht3l_error_t gxht3l_get_temp_humi(float *temp, float *humi)
{  
    // 读取数据
    uint8_t read_cmd[2] = {0xe0, 0x00};
    if (HAL_I2C_Master_Transmit(&hi2c2, 0x88, read_cmd, 2, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    uint8_t recv_buf[6];
    if (HAL_I2C_Master_Receive(&hi2c2, 0x89, recv_buf, 6, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    // 转换数据
    if (GXHT3L_Dat_To_Float(recv_buf, temp, humi) != 0) {
        return GXHT3L_ERROR_CRC;
    }
    
    return GXHT3L_OK;
}
