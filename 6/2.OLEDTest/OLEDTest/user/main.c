#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_oled.h"
#include "_freetype_impact_medium_r_normal__25_180_100_100_p_117_iso10646_1.h"
#include "_freetype_simsun_medium_r_normal__17_120_100_100_p_150_iso10646_1.h"
#include <stdio.h>

static PalI2C_HandleTypeDef hi2c1;
static PalOled_HandleTypeDef holed;

static const uint8_t rainnyIcon[] = {
	0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xfe, 0x0f, 0xf0, 0xfc, 0x07, 0xf0, 0xf8, 0x07, 0xf0, 0xf8, 
	0x00, 0x70, 0xe0, 0x00, 0x30, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x30, 0xe0, 0x00, 
	0x70, 0xff, 0xff, 0xf0, 0xf5, 0x6a, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xfe, 0xf0, 0xf5, 0xfa, 0xf0, 
	0xff, 0xff, 0xf0, 0xf5, 0xea, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0};

int main(void)
{
	PAL_Init();
	
	hi2c1.Init.I2Cx = I2C1;
	hi2c1.Init.I2C_ClockSpeed = 400000;
	hi2c1.Init.I2C_DutyCycle = I2C_DutyCycle_2;
	PAL_I2C_Init(&hi2c1);
	
	holed.Init.hi2c = &hi2c1;
	holed.Init.SA0 = RESET;
	PAL_OLED_Init(&holed);
	
	PAL_OLED_Clear(&holed);
	PAL_OLED_SetCursor(&holed, 4, 10);
	PAL_OLED_DrawString(&holed, "Date:21/12/23");
	//PAL_OLED_DrawBitmap(&holed, 128,64,ttsy_logo_image);
	
	PAL_OLED_SetCursor(&holed, 125, 2);
	PAL_OLED_LineTo(&holed, 108, 2);
	PAL_OLED_LineTo(&holed, 108, 4);
	PAL_OLED_LineTo(&holed, 106, 4);
	PAL_OLED_LineTo(&holed, 106, 7);
	PAL_OLED_LineTo(&holed, 108, 7);
	PAL_OLED_LineTo(&holed, 108, 9);
	PAL_OLED_LineTo(&holed, 125, 9);
	PAL_OLED_LineTo(&holed, 125, 2);
	
	PAL_OLED_SetPen(&holed, PEN_COLOR_TRANSPARENT, 1);
	PAL_OLED_SetBrush(&holed, BRUSH_WHITE);
	
	PAL_OLED_SetCursor(&holed, 110, 4);
	PAL_OLED_DrawRect(&holed, 2, 4);
	
	PAL_OLED_MoveCursorX(&holed, 3);
	PAL_OLED_DrawRect(&holed, 2, 4);
	
	PAL_OLED_MoveCursorX(&holed, 3);
	PAL_OLED_DrawRect(&holed, 2, 4);
	
	PAL_OLED_MoveCursorX(&holed, 3);
	PAL_OLED_DrawRect(&holed, 2, 4);
	
	PAL_OLED_MoveCursorX(&holed, 3);
	PAL_OLED_DrawRect(&holed, 2, 4);
	
	PAL_OLED_SetPen(&holed, PEN_COLOR_BLACK, 1);
	PAL_OLED_SetBrush(&holed, BRUSH_WHITE);
	PAL_OLED_SetCursor(&holed, 0, 42);
	PAL_OLED_DrawBitmap(&holed, 20, 20, rainnyIcon);
	
	PAL_OLED_SetPen(&holed, PEN_COLOR_WHITE, 1);
	PAL_OLED_SetBrush(&holed, BRUSH_TRANSPARENT);
	PAL_OLED_SetFont(&holed, &_freetype_simsun_medium_r_normal__17_120_100_100_p_150_iso10646_1);
	PAL_OLED_MoveCursor(&holed, 20 , 18);
	PAL_OLED_DrawString(&holed, "北京市海淀区");
	
	PAL_OLED_SendBuffer(&holed);
	
	while(1)
	{
		uint32_t currentTick = PAL_GetTick();
		
		currentTick = currentTick / 10;
		
		uint32_t ss = currentTick % 60;
		
		currentTick = currentTick / 60;
		
		uint32_t mm = currentTick % 60;
		
		currentTick = currentTick / 60;
		
		uint32_t hh = currentTick % 24;
		
		PAL_OLED_SetCursor(&holed, 2, 14);
		PAL_OLED_SetBrush(&holed, BRUSH_WHITE);
		PAL_OLED_DrawRect(&holed, 124, 24);
		
		char currentTimeStr[16];
		sprintf(currentTimeStr, "%02u : %02u : %02u", hh, mm, ss);
		
		PAL_OLED_SetPen(&holed, PEN_COLOR_BLACK, 1);
		PAL_OLED_SetBrush(&holed, BRUSH_TRANSPARENT);
		PAL_OLED_SetFont(&holed, &_freetype_impact_medium_r_normal__25_180_100_100_p_117_iso10646_1);
		PAL_OLED_SetCursor(&holed, (PAL_OLED_GetScreenWidth(&holed) -  PAL_OLED_GetStrWidth(&holed, currentTimeStr)) / 2, 36);
		PAL_OLED_DrawString(&holed, currentTimeStr);
		
		PAL_OLED_SendBuffer(&holed);
	}
}
