#include "oled.h"
#include "font.h"

int8_t oled_set_cursor(oled_config_t *oled, uint8_t Y, uint8_t X);
int8_t oled_initialize(oled_config_t *oled);
int8_t oled_clear_screen(oled_config_t *oled);
int8_t oled_show_char(oled_config_t *oled, uint8_t Line, uint8_t Column, char Char);
int8_t oled_show_string(oled_config_t *oled, uint8_t Line, uint8_t Column, char *String);
int8_t OLED_ShowGameBegin(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowNum(oled_config_t *oled, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
int8_t OLED_ShowGroud(oled_config_t *oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t Number);
int8_t OLED_ShowCloud(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowDino_Jump(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number);
int8_t OLED_ShowCactus1(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowCactus2(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowCactus3(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowGameOver(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int8_t OLED_ShowDino(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number);
int8_t OLED_ClearPicture(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

static bus_if_t oled_bus_instance = {0};

oled_config_t oled = {

	.bus = &oled_bus_instance,
	.oled_initialize = oled_initialize,
	.oled_show_char = oled_show_char,
	.oled_show_string = oled_show_string,
	.oled_clear_screen = oled_clear_screen,
	.OLED_ShowCactus1 = OLED_ShowCactus1,
	.OLED_ShowCactus2 = OLED_ShowCactus2,
	.OLED_ShowCactus3 = OLED_ShowCactus3,
	.OLED_ShowCloud = OLED_ShowCloud,
	.OLED_ShowDino_Jump = OLED_ShowDino_Jump,
	.OLED_ShowGameBegin = OLED_ShowGameBegin,
	.OLED_ShowGameOver = OLED_ShowGameOver,
	.OLED_ShowGroud = OLED_ShowGroud,
	.OLED_ShowNum = OLED_ShowNum,
	.OLED_ShowDino = OLED_ShowDino,
	.OLED_ClearPicture = OLED_ClearPicture
};
int8_t oled_bind_spi(oled_config_t *oled, spi_config_t *spi)
{
    oled->bus->ctx = spi;
    oled->bus->bus_initialize = (int8_t (*)(void *))spi->spi_initialize;
    oled->bus->bus_reset = (int8_t (*)(void *))spi->spi_reset;
    oled->bus->write_cmd = (int8_t (*)(void *, uint8_t))spi->write_cmd;
    oled->bus->write_data = (int8_t (*)(void *, uint8_t))spi->write_data;
    return 0;
}
int8_t oled_initialize(oled_config_t *oled)
{
    // 传递正确的参数：oled->bus->ctx 而不是 oled
    oled->bus->bus_initialize(oled->bus->ctx); // 总线初始化
    HAL_Delay(100);
    oled->bus->bus_reset(oled->bus->ctx);
    HAL_Delay(100);

    oled->bus->write_cmd(oled->bus->ctx, 0xAE);
    oled->bus->write_cmd(oled->bus->ctx, 0x00);
    oled->bus->write_cmd(oled->bus->ctx, 0x10);
    oled->bus->write_cmd(oled->bus->ctx, 0x40);
    oled->bus->write_cmd(oled->bus->ctx, 0x81);
    oled->bus->write_cmd(oled->bus->ctx, 0xCF);
    oled->bus->write_cmd(oled->bus->ctx, 0xA1);
    oled->bus->write_cmd(oled->bus->ctx, 0xC8);
    oled->bus->write_cmd(oled->bus->ctx, 0xA6);
    oled->bus->write_cmd(oled->bus->ctx, 0xA8);
    oled->bus->write_cmd(oled->bus->ctx, 0x3F);
    oled->bus->write_cmd(oled->bus->ctx, 0xD3);
    oled->bus->write_cmd(oled->bus->ctx, 0x00);
    oled->bus->write_cmd(oled->bus->ctx, 0xD5);
    oled->bus->write_cmd(oled->bus->ctx, 0x80);
    oled->bus->write_cmd(oled->bus->ctx, 0xD9);
    oled->bus->write_cmd(oled->bus->ctx, 0xF1);
    oled->bus->write_cmd(oled->bus->ctx, 0xDA);
    oled->bus->write_cmd(oled->bus->ctx, 0x12);
    oled->bus->write_cmd(oled->bus->ctx, 0xDB);
    oled->bus->write_cmd(oled->bus->ctx, 0x40);
    oled->bus->write_cmd(oled->bus->ctx, 0x20);
    oled->bus->write_cmd(oled->bus->ctx, 0x02);
    oled->bus->write_cmd(oled->bus->ctx, 0x8D);
    oled->bus->write_cmd(oled->bus->ctx, 0x14);
    oled->bus->write_cmd(oled->bus->ctx, 0xA4);
    oled->bus->write_cmd(oled->bus->ctx, 0xA6);
    oled->bus->write_cmd(oled->bus->ctx, 0xAF);
    return 0;
}

int8_t oled_set_cursor(oled_config_t *oled, uint8_t Y, uint8_t X)
{
    oled->bus->write_cmd(oled->bus->ctx, 0xB0 | Y);                 // 设置Y位置
    oled->bus->write_cmd(oled->bus->ctx, 0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    oled->bus->write_cmd(oled->bus->ctx, 0x00 | (X & 0x0F));        // 设置X位置低4位
    return 0;
}

int8_t oled_clear_screen(oled_config_t *oled)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++)
    {
        oled_set_cursor(oled, i, 0);
        for (j = 0; j < 128; j++)
        {
            oled->bus->write_data(oled->bus->ctx, 0); // 发送数据
        }
    }
    return 0;
}





int8_t oled_show_char(oled_config_t *oled, uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    oled_set_cursor(oled, (Line - 1) * 2, (Column - 1) * 8); // 设置光标位置在上半部分
    for (i = 0; i < 8; i++)
    {
        oled->bus->write_data(oled->bus->ctx, OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
    }
    oled_set_cursor(oled, (Line - 1) * 2 + 1, (Column - 1) * 8); // 设置光标位置在下半部分
    for (i = 0; i < 8; i++)
    {
        oled->bus->write_data(oled->bus->ctx, OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
    }
    return 0;
}

int8_t oled_show_string(oled_config_t *oled, uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		oled_show_char(oled, Line, Column + i, String[i]);
	}
	return 0;
}
int8_t OLED_ShowGameBegin(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		oled_set_cursor(oled, (y1 + i), x1);
		for (j = 0; j < (x2 - x1 + 1); j++)
		{
			oled->bus->write_data(oled->bus->ctx, OLED_GameBegin[i][j]); // 显示下半部分内容
		}
	}
	return 0;
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
 * @brief  OLED显示数字（十进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~4294967295
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
int8_t OLED_ShowNum(oled_config_t *oled, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		oled_show_char(oled, Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
	return 0;
}

int8_t OLED_ShowGroud(oled_config_t *oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t Number)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		oled_set_cursor(oled, (y1 + i), x1);
		for (j = 0; j < (x2 - x1 + 1); j++)
			oled->bus->write_data(oled->bus->ctx, OLED_Ground[i][j + Number]);
	}
	return 0;
}
int8_t OLED_ShowCloud(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		if (x1 > 100)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < 127 - x1; j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cloud[i][j]);
		}
		if (x1 >= 0 && x1 <= 100)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cloud[i][j]);
		}
		if (x1 < 0)
		{
			oled_set_cursor(oled, (y1 + i), 0);
			for (j = -x1; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cloud[i][j]);
		}
	}
	return 0;
}

int8_t OLED_ShowDino(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		oled_set_cursor(oled, (y1 + i), x1);
		for (j = 0; j < (x2 - x1 + 1); j++)
			oled->bus->write_data(oled->bus->ctx, OLED_Dino[Number][i][j]);
	}
	return 0;
}

int8_t OLED_ShowDino_Jump(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t Number)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		oled_set_cursor(oled, (y1 + i), x1);
		for (j = 0; j < (x2 - x1 + 1); j++)
			oled->bus->write_data(oled->bus->ctx, OLED_Dino_Jump[Number][i][j]);
	}
	return 0;
}

int8_t OLED_ShowCactus1(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		if (x1 > 119)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < 127 - x1; j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus1[i][j]);
		}
		if (x1 >= 0 && x1 <= 119)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus1[i][j]);
		}
		if (x1 < 0)
		{
			oled_set_cursor(oled, (y1 + i), 0);
			for (j = -x1; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus1[i][j]);
		}
	}
	return 0;
}

int8_t OLED_ShowCactus2(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		if (x1 > 111)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < 127 - x1; j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus2[i][j]);
		}
		if (x1 >= 0 && x1 <= 111)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus2[i][j]);
		}
		if (x1 < 0)
		{
			oled_set_cursor(oled, (y1 + i), 0);
			for (j = -x1; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus2[i][j]);
		}
	}
	return 0;
}

int8_t OLED_ShowCactus3(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		if (x1 > 111)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < 127 - x1; j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus3[i][j]);
		}
		if (x1 >= 0 && x1 <= 111)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for (j = 0; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus3[i][j]);
		}
		if (x1 < 0)
		{
			oled_set_cursor(oled, (y1 + i), 0);
			for (j = -x1; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx, OLED_Cactus3[i][j]);
		}
	}
	return 0;
}
int8_t OLED_ClearPicture(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t  j = 0;
	
	for(i = 0; i < (y2 - y1 + 1); i++)
	{
		if(x1 > 119)
		{
			oled_set_cursor(oled, (y1 + i), x1);
			for(j = 0; j < 127 - x1; j++)
				oled->bus->write_data(oled->bus->ctx,0);
		}
		if(x1 >= 0 && x1 <= 119)
		{
			oled_set_cursor(oled,(y1 + i), x1);
			for(j = 0; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx,0);
		}
		if(x1 < 0)
		{
			oled_set_cursor(oled,(y1 + i), 0);
			for(j = -x1; j < (x2 - x1 + 1); j++)
				oled->bus->write_data(oled->bus->ctx,0);
		}
	}
	return 0;
}
int8_t OLED_ShowGameOver(oled_config_t *oled, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0;
	uint8_t j = 0;

	for (i = 0; i < (y2 - y1 + 1); i++)
	{
		oled_set_cursor(oled, (y1 + i), x1);
		for (j = 0; j < (x2 - x1 + 1); j++)
			oled->bus->write_data(oled->bus->ctx, OLED_GameOver[i][j]);
	}
	return 0;
}
