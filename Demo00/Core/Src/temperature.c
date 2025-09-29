#include "temperature.h"
#include <stdio.h>

#define CRC8_POLYNOMIAL 0x31



/**
 * @brief CRC8У�麯��
 * @param message ��У�������
 * @param initial_value ��ʼֵ
 * @retval CRC8У��ֵ
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
 * @brief ��ԭʼ����ת��Ϊ��ʪ��ֵ
 * @param dat ԭʼ��������
 * @param temperature �¶�ָ��
 * @param humidity ʪ��ָ��
 * @retval 0: �ɹ�, 1: CRCУ��ʧ��
 */
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;

    /* У���¶����ݺ�ʪ�������Ƿ������ȷ */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;

    /* ת���¶����� */
    recv_temperature = ((uint16_t)dat[0] << 8) | dat[1];
    *temperature = -45 + 175 * ((float)recv_temperature / 65535);

    /* ת��ʪ������ */
    recv_humidity = ((uint16_t)dat[3] << 8) | dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);

    return 0;
}
/**
 * @brief ��ʼ��GXHT3L�������������߾��Ȳ�����
 * @retval gxht3l_error_t ������
 */
gxht3l_error_t gxht3l_init(void)
{
    // �����߾��Ȳ���
    uint8_t start_cmd[2] = {0x21, 0x30};
    if (HAL_I2C_Master_Transmit(&hi2c2, 0x88, start_cmd, 2, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    // �ȴ�������ɣ��߾�����Ҫ15ms��
    HAL_Delay(15);
    
    return GXHT3L_OK;
}
/**
 * @brief ��ȡ��ʪ������
 * @param temp �¶�ָ��
 * @param humi ʪ��ָ��
 * @retval gxht3l_error_t ������
 */
gxht3l_error_t gxht3l_get_temp_humi(float *temp, float *humi)
{  
    // ��ȡ����
    uint8_t read_cmd[2] = {0xe0, 0x00};
    if (HAL_I2C_Master_Transmit(&hi2c2, 0x88, read_cmd, 2, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    uint8_t recv_buf[6];
    if (HAL_I2C_Master_Receive(&hi2c2, 0x89, recv_buf, 6, 1000) != HAL_OK) {
        return GXHT3L_ERROR_I2C;
    }
    
    // ת������
    if (GXHT3L_Dat_To_Float(recv_buf, temp, humi) != 0) {
        return GXHT3L_ERROR_CRC;
    }
    
    return GXHT3L_OK;
}
