#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_button.h"

// PE4 -> 点亮 PB5   写  0 Button1
// PE3 -> 熄灭 PB5   写  1 Button2
PalButton_HandleTypeDef hButton1;
PalButton_HandleTypeDef hButton2;

static void OnButton1ReleasedCallback(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
}

static void OnButton2ReleasedCallback(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

int main(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	PAL_Init();
	
	// PC13 -> Out_OD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInitStruct);
	
	hButton1.Init.GPIOx = GPIOE;
	hButton1.Init.GPIO_Pin = GPIO_Pin_4;
	hButton1.Init.Button_Mode = Button_Mode_IPU;
	hButton1.Init.ButtonReleasedCallback = OnButton1ReleasedCallback; // 点亮
	PAL_Button_Init(&hButton1);
	
	hButton2.Init.GPIOx = GPIOE;
	hButton2.Init.GPIO_Pin = GPIO_Pin_3;
	hButton2.Init.Button_Mode = Button_Mode_IPU;
	hButton2.Init.ButtonReleasedCallback = OnButton2ReleasedCallback; // 熄灭
	PAL_Button_Init(&hButton2);
	
	while(1)
	{
		PAL_Button_Proc(&hButton1);
		PAL_Button_Proc(&hButton2);
	}
}
