#include "stm32f10x.h"
#include "stm32f10x_pal.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	PAL_Init();
//任务：两个按键分别控制led灯的亮灭
//	1.初始化两个按键的GPIO PE3 PE4
//	1.1初始化时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
//	1.2初始化两个按键的GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
//	2.初始化afio，将gpio引脚配置到exti线上
//	2.1初始化时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//	2.2初始化两个按键的AFIO
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
	
//	3.配置exti
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line3;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_Init(&EXTI_InitStruct);
	
//	4.配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
//	5.配置led灯 PB5 低电平亮
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	while(1)
	{
	}

}

//	6.编写中断服务函数
void EXTI3_IRQHandler(void)  //点亮LED灯
{
	//清楚标志位
	EXTI_ClearITPendingBit(EXTI_Line3);
	//中断处理   PB5 低电平亮
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	
}
void EXTI4_IRQHandler(void)  //熄灭LED灯
{
	//清楚标志位
	EXTI_ClearITPendingBit(EXTI_Line4);
	//中断处理   PB5 低电平亮
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	
}
