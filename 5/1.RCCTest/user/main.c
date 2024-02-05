#include "stm32f10x.h"
#include "stm32f10x_pal.h"

int main(void)
{
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_2);
	
	// 开启HSE
	RCC_HSEConfig(RCC_HSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	
	
	// 配置锁相环
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	// 开启锁相环
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
	
	// 将SYSCLK设置为锁相环的输出
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	// 设置AHB分频器的分频系数
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
	// 设置APB1分频器的分频系数
	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	// 设置APB2分频器的分频系数
	RCC_PCLK2Config(RCC_HCLK_Div2);
	
	// 关闭HSI
	RCC_HSICmd(DISABLE);
	
	PAL_Init();
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	uint32_t i;
	
	while(1)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		for(i=0;i<12000000;i++); // 1s 72000000/6 = 1200000
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		for(i=0;i<12000000;i++); // 1s
	}
}
