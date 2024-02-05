#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_button.h"

PalButton_HandleTypeDef hButton1; // h - Handle

static void OnButton1Released(void)
{
	// PC13亮灭状态切换的代码
	if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5) == Bit_SET)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
	}
}

int main(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	PAL_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInitStruct);
	

	hButton1.Init.GPIOx = GPIOE;
	hButton1.Init.GPIO_Pin = GPIO_Pin_4;
	hButton1.Init.Button_Mode = Button_Mode_IPU;
	hButton1.Init.ButtonReleasedCallback = OnButton1Released;
	
	PAL_Button_Init(&hButton1);
	
	while(1)
	{
		PAL_Button_Proc(&hButton1);
	}
}
