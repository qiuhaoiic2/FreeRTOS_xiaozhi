#ifndef _USER_OLED_H
#define _USER_OLED_H

#include "stdint.h"


    
void oled_initialize(void);
void oled_clear_screen(void);
void oled_show_char(uint8_t Line, uint8_t Column, char Char, uint8_t FontSize);
void oled_show_string(uint8_t Line, uint8_t Column, char *String, uint8_t FontSize);
void oled_show_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_chinese(uint8_t Line, uint8_t Column, uint8_t Index, uint8_t FontSize);
void oled_show_float(uint8_t Line, uint8_t Column, float Number, uint8_t FontSize);
void oled_show_temperature(uint8_t Line, uint8_t Column, float Temperature, uint8_t FontSize);
void oled_show_humidity(uint8_t Line, uint8_t Column, float Humidity, uint8_t FontSize);



   





#endif

