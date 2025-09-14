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
    SPI_TypeDef         *Instance;

    int8_t (*spi_reset)(spi_config_t *spi);
    int8_t (*spi_initialize)(spi_config_t *spi);
    int8_t (*write_cmd)(spi_config_t *spi, uint8_t cmd);
    int8_t (*write_data)(spi_config_t *spi, const uint8_t data);

};
extern spi_config_t user_spi3;

#endif
