#include "spi.h"

static int8_t spi_initialize(spi_config_t *spi);
static int8_t spi_transmit(spi_config_t *spi,uint8_t data);
static int8_t spi_write_data(spi_config_t *spi, uint8_t data);
static int8_t spi_write_cmd(spi_config_t *spi, uint8_t data);
static int8_t spi_reset(spi_config_t *spi);

static SPI_HandleTypeDef hspi3;

gpio_config_t sda_config = {
    .GpioPort = GPIOB,
    .Pin = GPIO_PIN_5,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH
};
gpio_config_t clk_config = {
    .GpioPort = GPIOB,
    .Pin = GPIO_PIN_3,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH
};
gpio_config_t dc_config = {
    .GpioPort = GPIOB,
    .Pin = GPIO_PIN_4,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH
};
gpio_config_t res_config = {
    .GpioPort = GPIOB,
    .Pin = GPIO_PIN_8,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_HIGH
};


spi_config_t user_spi3 = {
    .spi_dc = &dc_config,
    .spi_res = &res_config,
    .spi_scl = &clk_config,
    .spi_sda = &sda_config,
    .hspi    = &hspi3,
    .Instance = SPI3,
    .spi_initialize = spi_initialize,
    .write_cmd = spi_write_cmd,
    .write_data = spi_write_data,
    .spi_reset = spi_reset
};
static int8_t spi_reset(spi_config_t *spi)
{
    GPIO_Write_High(spi->spi_res); // 先设置为高电平
    HAL_Delay(10);
    GPIO_Write_Low(spi->spi_res);  // RES reset (低电平复位)
    HAL_Delay(10);
    GPIO_Write_High(spi->spi_res); // RES set (复位完成)
    HAL_Delay(10);
    return 0;
}
static int8_t spi_initialize(spi_config_t *spi)
{
    if (spi == NULL) return -1;
    /* 根据实例打开对应外设时钟 */
    if      (spi->Instance == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    else if (spi->Instance == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    else if (spi->Instance == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();
    else return -2;

    __HAL_RCC_AFIO_CLK_ENABLE();

    GPIO_Init_Universal(spi->spi_sda);
    GPIO_Init_Universal(spi->spi_scl);
    GPIO_Init_Universal(spi->spi_res);
    GPIO_Init_Universal(spi->spi_dc);
    
    spi->hspi->Instance = spi->Instance;
    spi->hspi->Init.Mode = SPI_MODE_MASTER;
    spi->hspi->Init.Direction = SPI_DIRECTION_1LINE;
    spi->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    spi->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    spi->hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    spi->hspi->Init.NSS = SPI_NSS_SOFT;
    spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    spi->hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    spi->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi->hspi->Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(spi->hspi) != HAL_OK)
    {
        return -1;
    }
    return 0;
}
static int8_t spi_transmit(spi_config_t *spi,uint8_t data)
{
    HAL_StatusTypeDef errorcode = HAL_OK;

    if (spi == NULL) return -1;
    errorcode = HAL_SPI_Transmit(spi->hspi,&data,1,0xff);

    return (int8_t)errorcode;
}

static int8_t spi_write_data(spi_config_t *spi, uint8_t data)
{
    GPIO_Write_High(spi->spi_dc);
    spi_transmit(spi, data);
    return 0;
}
static int8_t spi_write_cmd(spi_config_t *spi, uint8_t data)
{
    GPIO_Write_Low(spi->spi_dc);
    spi_transmit(spi, data);
    return 0;
}
