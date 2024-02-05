#include "app_cmd.h"
#include "stm32f10x_pal_usart.h"
#include <string.h>
#include "app_blinky_led.h"
#include <stdlib.h>

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
	hUSART1.Init.TxBufferSize = 128;
	hUSART1.Init.RxBufferSize = 128;
	hUSART1.Init.Advanced.LineSeparator = LineSeparator_CRLF; // \r\n
	PAL_USART_Init(&hUSART1);
}

void App_Cmd_Proc(void)
{
	char cmdLine[64];
	char *ptr;
	const char *cmdName; // 指令名称
	
	if(PAL_USART_ReadLine(&hUSART1, cmdLine, sizeof(cmdLine)/sizeof(char), 0)>0)
	{
		cmdLine[strlen(cmdLine)-2] = '\0'; // 删除行尾的\r\n
		ptr = cmdLine;
		cmdName = ptr;
		ptr = strchr(cmdLine, ' ');
		
		if(ptr != 0)
		{
			*ptr = '\0';
			ptr++;
		}
		
		if(strcmp(cmdName, "SetBlinkParam")==0)
		{
			const char *arg1, *arg2;
			
			// 解析第1个参数
			arg1 = ptr;
			ptr = strchr(ptr, ' ');
			*ptr = '\0';
			ptr++;
			
			// 解析第2个参数
			arg2 = ptr;
			
			App_BlinkyLED_SetOnTime(atoi(arg1)); //atoi()将字符串转化为整数
			App_BlinkyLED_SetOffTime(atoi(arg2));
			
			// OK\r\n\r\n
			PAL_USART_SendString(&hUSART1, "OK\r\n\r\n");
		} // 修改闪灯参数
		else if(strcmp(cmdName, "GetBlinkParam")==0)
		{
			// OK\r\nOn=%d, Off=%d\r\n
			PAL_USART_Printf(&hUSART1, "OK\r\nOn=%d, Off=%d\r\n", App_BlinkyLED_GetOnTime(),App_BlinkyLED_GetOffTime());
		} // 获取闪灯参数
		else // ERROR, cmd name not supported. 
		{
			PAL_USART_Printf(&hUSART1, "ERROR, cmd name not supported\r\n\r\n");
		}
	}
}

void USART1_IRQHandler(void)
{
	PAL_USART_IRQHandler(&hUSART1);
}
