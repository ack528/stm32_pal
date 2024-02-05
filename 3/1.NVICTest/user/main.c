#include "stm32f10x.h"
#include "stm32f10x_pal.h"

int main(void)
{
	NVIC_InitTypeDef NVICInitStruct;
	
	// 中断优先级分组 分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	PAL_Init();
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVICInitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVICInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVICInitStruct);
	
	while(1)
	{
	}
}

void USART1_IRQHandler(void)
{
	// 具体的中断响应代码
}
