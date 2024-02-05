#include "stm32f10x.h"
#include "stm32f10x_pal.h"

//按键(KEY)是PE4 按下低电平，松开高电平
//LED灯为PB5 低电平点亮

int main(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	uint8_t current = Bit_SET;
	uint8_t previous = Bit_SET;
	
	PAL_Init();
	
	// PC13 Out_OD
	// GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOC的时钟
	
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInitStruct);
	
	// PA0  IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); // GPIOA的时钟
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIOInitStruct);
	
	while(1)
	{
		previous = current; 
		
		current = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
		
		if(current != previous) 
		{
			PAL_Delay(10); //消抖
			
			current = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
			
			if(current != previous)
			{
				if(current == Bit_RESET) // 按钮按下
				{
					
				}
				else // 按钮松开
				{
					// ODR
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
		}
	}
}
