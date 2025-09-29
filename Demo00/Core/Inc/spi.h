#ifndef _USER_SPI_H
#define _USER_SPI_H

#include "gpio.h"
#include "stm32f1xx_hal.h"

typedef struct spi_config_t spi_config_t;
struct spi_config_t
{
    gpio_config_t *spi_scl;
    gpio_config_t *spi_dc;
    gpio_config_t *spi_sda;
    gpio_config_t *spi_res;
    SPI_HandleTypeDef *hspi;
    SPI_TypeDef *Instance;
};
extern spi_config_t user_spi3;
int8_t spi_initialize(spi_config_t *spi);
int8_t spi_reset(spi_config_t *spi);

int8_t spi_write_data(spi_config_t *spi, uint8_t data);
int8_t spi_write_cmd(spi_config_t *spi, uint8_t data);

int8_t spi_write_datas(spi_config_t *spi, uint8_t *data, uint16_t size);
int8_t spi_write_cmds(spi_config_t *spi, uint8_t *data, uint16_t size);
#endif
