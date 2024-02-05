#include "app_cmd.h"
#include "stm32f10x_pal_usart.h"
#include "pal_str_codec.h"
#include <string.h>
#include "app_blinky_led.h"

static PalUSART_HandleTypeDef hUSART1;
static PalStrCodec_HandleTypeDef hStrCodec;

void App_Cmd_Init(void)
{
	hUSART1.Init.USARTx = USART1;
	hUSART1.Init.BaudRate = 115200;
	hUSART1.Init.USART_WordLength = USART_WordLength_8b;
	hUSART1.Init.USART_Parity = USART_Parity_No;
	hUSART1.Init.USART_StopBits = USART_StopBits_1;
	hUSART1.Init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	hUSART1.Init.USART_IRQ_PreemptionPriority = 0;
	hUSART1.Init.USART_IRQ_SubPriority = 0;
	hUSART1.Init.TxBufferSize = 128;
	hUSART1.Init.RxBufferSize = 128;
	hUSART1.Init.Advanced.LineSeparator = LineSeparator_CRLF; // \r\n
	PAL_USART_Init(&hUSART1);
	
	hStrCodec.Init.hUSART = &hUSART1;
	PAL_StrCodec_Init(&hStrCodec);
}

void App_Cmd_Proc(void)
{
	if(PAL_StrCodec_Receive(&hStrCodec) == SUCCESS)
	{
		const char *cmdName = PAL_StrCodec_GetName(&hStrCodec);
		int32_t onTime, offTime;
		
		if(strcmp(cmdName, "SetBlinkParam")==0)
		{
			if(PAL_StrCodec_GetNumberOfArgs(&hStrCodec) != 2)
			{
				// ERROR, incorrect num of args\r\n\r\n
				PAL_USART_SendString(&hUSART1, "ERROR, incorrect num of args\r\n\r\n");
			}
			else
			{
				if(PAL_StrCodec_ReadArgInt(&hStrCodec, 0, &onTime) == ERROR || 
				   PAL_StrCodec_ReadArgInt(&hStrCodec, 1, &offTime) == ERROR)
				{
					PAL_USART_SendString(&hUSART1, "ERROR, invalid arg\r\n\r\n");
				}
				else
				{
					App_BlinkyLED_SetOnTime(onTime);
					App_BlinkyLED_SetOffTime(offTime);
					
					// OK\r\n\r\n
					PAL_USART_SendString(&hUSART1, "OK\r\n\r\n");
				}
			}
		}
		else if(strcmp(cmdName, "GetBlinkParam")==0)
		{
			onTime = App_BlinkyLED_GetOnTime();
			offTime = App_BlinkyLED_GetOffTime();
			// OK\r\nOn=%d,Off=%d\r\n
			PAL_USART_Printf(&hUSART1, "OK\r\nOn=%d,Off=%d\r\n", onTime, offTime);
		}
		else 
		{
			// ERROR, unsupported cmd name\r\n\r\n
			PAL_USART_SendString(&hUSART1, "ERROR, unsupported cmd name\r\n\r\n");
		}
	}
}

void USART1_IRQHandler(void)
{
	PAL_USART_IRQHandler(&hUSART1);
}
