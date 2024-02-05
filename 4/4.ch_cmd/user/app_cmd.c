#include "app_cmd.h"
#include "stm32f10x_pal_usart.h"
#include "app_blinky_led.h"

static PalUSART_HandleTypeDef hUSART1;

void App_Cmd_Init(void)
{
	hUSART1.Init.USARTx = USART1;
	hUSART1.Init.BaudRate = 115200;
	hUSART1.Init.USART_WordLength = USART_WordLength_8b;
	hUSART1.Init.USART_StopBits = USART_StopBits_1;
	hUSART1.Init.USART_Parity = USART_Parity_No;
	hUSART1.Init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	hUSART1.Init.USART_IRQ_PreemptionPriority = 0;
	hUSART1.Init.USART_IRQ_SubPriority = 0;
	hUSART1.Init.RxBufferSize = 128;
	hUSART1.Init.TxBufferSize = 128;
	PAL_USART_Init(&hUSART1);
}

void App_Cmd_Proc(void) //会出现回复3次相同数据的bug
{
	int16_t tmp;
	
	tmp = PAL_USART_ReceiveByte(&hUSART1, 0);
	
	if(tmp >=0)
	{
		char c = (char)tmp;
		switch(c)
		{
			case 'q': App_BlinkyLED_SetOnTime(App_BlinkyLED_GetOnTime()+10); break;
			case 'w': App_BlinkyLED_SetOnTime(App_BlinkyLED_GetOnTime()-10); break;
			case 'e': App_BlinkyLED_SetOffTime(App_BlinkyLED_GetOffTime()+10); break;
			case 'r': App_BlinkyLED_SetOffTime(App_BlinkyLED_GetOffTime()-10); break;
		}
		PAL_USART_Printf(&hUSART1, "on=%dms, off=%dms\r\n", App_BlinkyLED_GetOnTime(), App_BlinkyLED_GetOffTime());
	}
}

void USART1_IRQHandler(void)
{
	PAL_USART_IRQHandler(&hUSART1);
}
