#include "oled.h"
#include "oledfont.h"
#include "spi.h"
#include <string.h>
#include <stdio.h>


void oled_initialize()
{
	spi_initialize(&user_spi3);
	HAL_Delay(100);
	spi_reset(&user_spi3);
	HAL_Delay(100);
	spi_write_cmd(&user_spi3, 0xAE);
	spi_write_cmd(&user_spi3, 0x00);
	spi_write_cmd(&user_spi3, 0x10);
	spi_write_cmd(&user_spi3, 0x40);
	spi_write_cmd(&user_spi3, 0x81);
	spi_write_cmd(&user_spi3, 0xCF);
	spi_write_cmd(&user_spi3, 0xA1);
	spi_write_cmd(&user_spi3, 0xC8);
	spi_write_cmd(&user_spi3, 0xA6);
	spi_write_cmd(&user_spi3, 0xA8);
	spi_write_cmd(&user_spi3, 0x3F);
	spi_write_cmd(&user_spi3, 0xD3);
	spi_write_cmd(&user_spi3, 0x00);
	spi_write_cmd(&user_spi3, 0xD5);
	spi_write_cmd(&user_spi3, 0x80);
	spi_write_cmd(&user_spi3, 0xD9);
	spi_write_cmd(&user_spi3, 0xF1);
	spi_write_cmd(&user_spi3, 0xDA);
	spi_write_cmd(&user_spi3, 0x12);
	spi_write_cmd(&user_spi3, 0xDB);
	spi_write_cmd(&user_spi3, 0x40);
	spi_write_cmd(&user_spi3, 0x20);
	spi_write_cmd(&user_spi3, 0x02);
	spi_write_cmd(&user_spi3, 0x8D);
	spi_write_cmd(&user_spi3, 0x14);
	spi_write_cmd(&user_spi3, 0xA4);
	spi_write_cmd(&user_spi3, 0xA6);
	spi_write_cmd(&user_spi3, 0xAF);
	oled_clear_screen();
}

void oled_set_cursor(uint8_t Y, uint8_t X)
{
	spi_write_cmd(&user_spi3, 0xB0 | Y);				 // 设置Y位置
	spi_write_cmd(&user_spi3, 0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
	spi_write_cmd(&user_spi3, 0x00 | (X & 0x0F));		 // 设置X位置低4位
}

void oled_clear_screen()
{
	uint8_t i;
	uint8_t buf[128];
	for (i = 0; i < 8; i++)
	{
		oled_set_cursor(i, 0);
		memset(buf, 0x00, 128);
		spi_write_datas(&user_spi3, buf, 128);
		spi_write_datas(&user_spi3, buf, 128);
	}
}

/**
 * @brief  OLED次方函数
 * @retval 返回值等于X的Y次方
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED显示字符（支持多种字体大小）
 * @param  Line 起始行位置，范围：1~8
 * @param  Column 起始列位置，范围：1~16
 * @param  Char 要显示的字符
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_char(uint8_t Line, uint8_t Column, char Char, uint8_t FontSize)
{
	uint8_t char_index = Char - ' ';

	switch (FontSize)
	{
	case 6: // 6x8字体 (asc2_0806)
	{
		uint8_t x = (Column - 1) * 6;
		uint8_t y = Line - 1;
		const uint8_t *font_data = asc2_0806[char_index];

		oled_set_cursor(y, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data, 6);
		break;
	}

	case 12: // 12x12字体 (asc2_1206)
	{
		uint8_t x = (Column - 1) * 6;
		uint8_t y = (Line - 1) * 2;
		const uint8_t *font_data = asc2_1206[char_index];

		// 显示上半部分
		oled_set_cursor(y, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data, 6);

		// 显示下半部分
		oled_set_cursor(y + 1, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data + 6, 6);
		break;
	}

	case 16: // 16x16字体 (asc2_1608)
	{
		uint8_t x = (Column - 1) * 8;
		uint8_t y = (Line - 1) * 2;
		const uint8_t *font_data = asc2_1608[char_index];

		// 显示上半部分
		oled_set_cursor(y, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data, 8);

		// 显示下半部分
		oled_set_cursor(y + 1, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data + 8, 8);
		break;
	}

	case 24: // 24x24字体 (asc2_2412)
	{
		uint8_t x = (Column - 1) * 12;
		uint8_t y = (Line - 1) * 3;
		const uint8_t *font_data = asc2_2412[char_index];

		// 显示上部分
		oled_set_cursor(y, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data, 12);

		// 显示中部分
		oled_set_cursor(y + 1, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data + 12, 12);

		// 显示下部分
		oled_set_cursor(y + 2, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data + 24, 12);
		break;
	}

	default:
		// 默认使用8x8字体
		oled_show_char(Line, Column, Char, 8);
		break;
	}
}

/**
 * @brief  OLED显示字符串（支持多种字体大小）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  String 要显示的字符串
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_string(uint8_t Line, uint8_t Column, char *String, uint8_t FontSize)
{
	uint8_t i;
	uint8_t char_width;

	// 根据字体大小确定字符宽度
	switch (FontSize)
	{
	case 6:
		char_width = 1;
		break; // 6x8字体，占1列
	case 12:
		char_width = 1;
		break; // 12x12字体，占1列（6像素宽度）
	case 16:
		char_width = 1;
		break; // 16x16字体，占1列（8像素宽度）
	case 24:
		char_width = 2;
		break; // 24x24字体，占2列（12像素宽度）
	default:
		char_width = 1;
		break;
	}

	for (i = 0; String[i] != '\0'; i++)
	{
		oled_show_char(Line, Column + i * char_width, String[i], FontSize);
	}
}

/**
 * @brief  OLED显示数字（支持多种字体大小）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  Number 要显示的数字
 * @param  Length 要显示数字的长度
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint8_t char_width;

	// 根据字体大小确定字符宽度
	switch (FontSize)
	{
	case 6:
		char_width = 1;
		break;
	case 12:
		char_width = 1;
		break;
	case 16:
		char_width = 1;
		break;
	case 24:
		char_width = 2;
		break;
	default:
		char_width = 1;
		break;
	}

	for (i = 0; i < Length; i++)
	{
		oled_show_char(Line, Column + i * char_width,
					   Number / OLED_Pow(10, Length - i - 1) % 10 + '0',
					   FontSize);
	}
}

/**
 * @brief  显示中文字符（支持多种尺寸）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  Index 中文字符在字库中的索引
 * @param  FontSize 字体大小：16, 24, 32, 64
 * @retval 无
 */
void oled_show_chinese(uint8_t Line, uint8_t Column, uint8_t Index, uint8_t FontSize)
{
	uint8_t x, y, i;
	const uint8_t *font_data;

	switch (FontSize)
	{
	case 16: // 16x16中文字体 (Hzk1)
	{
		x = (Column - 1) * 16;
		y = (Line - 1) * 2;
		font_data = Hzk1[Index];

		// 显示上半部分
		oled_set_cursor(y, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data, 16);

		// 显示下半部分
		oled_set_cursor(y + 1, x);
		spi_write_datas(&user_spi3, (uint8_t *)font_data + 16, 16);
		break;
	}

	case 24: // 24x24中文字体 (Hzk2)
	{
		x = (Column - 1) * 24;
		y = (Line - 1) * 3;
		font_data = Hzk2[Index];

		for (i = 0; i < 3; i++)
		{
			oled_set_cursor(y + i, x);
			spi_write_datas(&user_spi3, (uint8_t *)font_data + i * 24, 24);
		}
		break;
	}

	case 32: // 32x32中文字体 (Hzk3)
	{
		x = (Column - 1) * 32;
		y = (Line - 1) * 4;
		font_data = Hzk3[Index];

		for (i = 0; i < 4; i++)
		{
			oled_set_cursor(y + i, x);
			spi_write_datas(&user_spi3, (uint8_t *)font_data + i * 32, 32);
		}
		break;
	}

	case 64: // 64x64中文字体 (Hzk4)
	{
		x = (Column - 1) * 64;
		y = (Line - 1) * 8;
		font_data = Hzk4[Index];

		for (i = 0; i < 8; i++)
		{
			oled_set_cursor(y + i, x);
			spi_write_datas(&user_spi3, (uint8_t *)font_data + i * 64, 64);
		}
		break;
	}
	}
}

/**
 * @brief  OLED显示浮点数（简化版，自动格式化）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  Number 要显示的浮点数
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_float(uint8_t Line, uint8_t Column, float Number, uint8_t FontSize)
{
	char float_str[16];

	// 将浮点数转换为字符串（保留2位小数）
	sprintf(float_str, "%.2f", Number);

	// 显示字符串
	oled_show_string(Line, Column, float_str, FontSize);
}

/**
 * @brief  OLED显示温度（带单位）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  Temperature 温度值
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_temperature(uint8_t Line, uint8_t Column, float Temperature, uint8_t FontSize)
{
	char temp_str[16];

	// 格式化温度字符串
	sprintf(temp_str, "%.1fC", Temperature);

	// 显示温度字符串
	oled_show_string(Line, Column, temp_str, FontSize);
}

/**
 * @brief  OLED显示湿度（带单位）
 * @param  Line 起始行位置
 * @param  Column 起始列位置
 * @param  Humidity 湿度值
 * @param  FontSize 字体大小：6, 12, 16, 24
 * @retval 无
 */
void oled_show_humidity(uint8_t Line, uint8_t Column, float Humidity, uint8_t FontSize)
{
	char humi_str[16];

	// 格式化湿度字符串
	sprintf(humi_str, "%.1f%%", Humidity);

	// 显示湿度字符串
	oled_show_string(Line, Column, humi_str, FontSize);
}
