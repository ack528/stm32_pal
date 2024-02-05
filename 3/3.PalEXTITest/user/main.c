#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_exti.h"
//PE4 KEY0
//PE3 KEY1
//PB5 LED0


static PalEXTI_HandleTypeDef hEXTI3;

static PalEXTI_HandleTypeDef hEXTI4;

static void OnEXTI3Triggered(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
}

static void OnEXTI4Triggered(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

int main(void)
{
	PAL_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIOInitStruct;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInitStruct);

	
	hEXTI3.Init.GPIO.GPIOx = GPIOE;
	hEXTI3.Init.GPIO.GPIO_Pin = GPIO_Pin_3;
	hEXTI3.Init.GPIO.GPIO_Mode = GPIO_Mode_IPU;
	hEXTI3.Init.Exti.EXTI_Mode = EXTI_Mode_Interrupt;
	hEXTI3.Init.Exti.EXTI_Trigger = EXTI_Trigger_Rising;
	hEXTI3.Init.Interrupt.PreemptionPriority = 0;
	hEXTI3.Init.Interrupt.SubPriority = 0;
	hEXTI3.Init.CallbackFn = OnEXTI3Triggered; 
	PAL_EXTI_Init(&hEXTI3);
	PAL_EXTI_LineCmd(&hEXTI3, ENABLE);
	
	
	hEXTI4.Init.GPIO.GPIOx = GPIOE;
	hEXTI4.Init.GPIO.GPIO_Pin = GPIO_Pin_4;
	hEXTI4.Init.GPIO.GPIO_Mode = GPIO_Mode_IPU;
	hEXTI4.Init.Exti.EXTI_Mode = EXTI_Mode_Interrupt;
	hEXTI4.Init.Exti.EXTI_Trigger = EXTI_Trigger_Rising;
	hEXTI4.Init.Interrupt.PreemptionPriority = 0;
	hEXTI4.Init.Interrupt.SubPriority = 0;
	hEXTI4.Init.CallbackFn = OnEXTI4Triggered; 
	PAL_EXTI_Init(&hEXTI4);
	PAL_EXTI_LineCmd(&hEXTI4, ENABLE);
	
	while(1)
	{
	}
}

void EXTI3_IRQHandler(void)
{
	PAL_EXTI_IRQHandler(&hEXTI3);
}
void EXTI4_IRQHandler(void)
{
	PAL_EXTI_IRQHandler(&hEXTI4);
}
