#include "app_blinky_led.h"
#include "stm32f10x_pal.h"

static uint32_t onTime = 10;
static uint32_t offTime = 100;
static uint8_t stage = 0;
static uint64_t lastToggleTime = 0;

void App_BlinkyLED_SetOnTime(uint32_t Val)
{
	onTime = Val;
}

void App_BlinkyLED_SetOffTime(uint32_t Val)
{
	offTime = Val;
}

uint32_t App_BlinkyLED_GetOnTime(void)
{
	return onTime;
}

uint32_t App_BlinkyLED_GetOffTime(void)
{
	return offTime;
}

void App_BlinkyLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void App_BlinkyLED_Proc(void)
{
	switch(stage)
	{
		case 0: // 当前是熄灭状态
			if(PAL_GetTick() >= lastToggleTime + offTime)
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				lastToggleTime = PAL_GetTick();
				stage = 1;
			}
			break;
		case 1: // 当前是点亮状态
			if(PAL_GetTick() >= lastToggleTime + onTime)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				lastToggleTime = PAL_GetTick();
				stage = 0;
			}
			break;
		default:
			break;
	}
}

