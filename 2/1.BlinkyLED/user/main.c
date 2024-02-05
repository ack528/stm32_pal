#include "stm32f10x.h"
#include "stm32f10x_pal.h"

int main(void)
{	
	GPIO_InitTypeDef GPIOInitStruct;
	
	PAL_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIOInitStruct);
	
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);// 开漏，1 - 熄灭 ， 0- 点亮 Bit_SET
//	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
//	GPIO_Write(GPIOC, 0x2000); // 1 - 熄灭
//	GPIO_Write(GPIOC, 0); // 点亮
//	GPIO_SetBits(GPIOC, GPIO_Pin_13);
//	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
	
	while(1)
	{
		PAL_Delay(100);
		
		if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5) == Bit_RESET)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
		}
		else
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
		}

	}
}
