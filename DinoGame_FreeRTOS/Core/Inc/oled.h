#ifndef _USER_OLED_H
#define _USER_OLED_H

#include "spi.h"

typedef struct
{
    void *ctx;   /* 指向 spi_config_t 或 i2c_config_t */
    int8_t (*bus_reset)(void *ctx);
    int8_t (*bus_initialize)(void *ctx);
    int8_t (*write_cmd)(void *ctx, uint8_t cmd);
    int8_t (*write_data)(void *ctx,uint8_t data);
} bus_if_t;

typedef struct oled_config_t oled_config_t;
struct oled_config_t
{
    bus_if_t *bus;
    
    int8_t (*oled_initialize)(oled_config_t *oled);
    int8_t (*oled_clear_screen)(oled_config_t *oled);
    int8_t (*oled_show_char)(oled_config_t *oled,uint8_t Line, uint8_t Column, char Char);
    int8_t (*oled_show_string)(oled_config_t *oled,uint8_t Line, uint8_t Column, char *String);
    int8_t (*OLED_ShowGameBegin)(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowNum)(oled_config_t *oled,uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
    int8_t (*OLED_ShowGroud)(oled_config_t *oled,uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t Number);
    int8_t (*OLED_ShowCloud)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowDino_Jump)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number);
    int8_t (*OLED_ShowCactus1)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowCactus2)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowCactus3)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowGameOver)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    int8_t (*OLED_ShowDino)(oled_config_t *oled,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number);
    int8_t (*OLED_ClearPicture)(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

   
};
extern oled_config_t oled;

int8_t oled_bind_spi(oled_config_t *oled,spi_config_t *spi);





#endif

