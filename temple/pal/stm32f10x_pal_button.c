/**
  ******************************************************************************
  * @file    stm32f10x_pal_button.c
  * @author  铁头山羊stm32工作组
  * @version V1.0.0
  * @date    2022年11月25日
  * @brief   按钮驱动
  ******************************************************************************
  * @attention
  * Copyright (c) 2022 东莞市明玉科技有限公司. 保留所有权力.
  ******************************************************************************
  ==============================================================================
	                                  主要功能
  ==============================================================================
	1. 支持使用任意的IO引脚作为按钮的驱动引脚
	2. 支持“内部上拉电阻”和“外部上拉电阻”两种接法
	3. 内置软件消抖功能
	
  ==============================================================================
	                                  使用方法
  ==============================================================================
  1. 声明一个按钮的句柄。比如：
     Button_HandleTypeDef hButton;
	2. 使用hButton.Button_Port和hButton.Button_Pin选择驱动按钮的IO
  3. 使用hButton.Button_Mode选择按钮的接线模式
	4. 使用hButton.ButtonPressedCallback定义按钮按下时的处理代码；
	   使用hButton.ButtonReleasedCallback定义按钮松开时的处理代码。
		 给hButton.ButtonPressedCallback或者hButton.ButtonReleasedCallback赋0，表示按钮无动作
	5. 调用PAL_Button_Init进行初始化。比如：
	   PAL_Button_Init(&hButton);
	6. 在主程序的while循环中调用任务进程，PAL_Button_Proc。比如：
	   while(1)
		 {
		   ...
		   PAL_Button_Proc(&hButton);
			 ...
		 }
*/

#include "stm32f10x_pal.h"
#include "stm32f10x_pal_button.h"

#define Button_ActiveLevel(Button) Bit_RESET
#define Button_InActiveLevel(Button) Bit_SET
#define Button_GetState(Button) GPIO_ReadInputDataBit(Button->Init.GPIOx, Button->Init.GPIO_Pin)


/*
 * @简介：用于初始化按钮的驱动
 * @参数：
 *       Handle - 按钮的句柄
 * 
 * @返回值：ERROR表示初始化失败，SUCCESS表示初始化成功
 */
ErrorStatus PAL_Button_Init(PalButton_HandleTypeDef *Handle)
{
	GPIO_InitTypeDef gpio_init_struct;
	
	RCC_GPIOx_ClkCmd(Handle->Init.GPIOx, ENABLE);
	
	gpio_init_struct.GPIO_Pin = Handle->Init.GPIO_Pin;
	if(Handle->Init.Button_Mode == BUTTON_Mode_EPU)
	{
		gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	}
	else if(Handle->Init.Button_Mode == BUTTON_Mode_IPU)
	{
		gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	}
	
	GPIO_Init(Handle->Init.GPIOx, &gpio_init_struct);
	
	Handle->PendingTime = 0;
	Handle->ChangePending = 0;
	Handle->LastState = Button_InActiveLevel(Handle);
	
	return SUCCESS;
}

/*
 * @简介：按钮的进程函数
 * @参数：
 *        Handle - 按钮的句柄
 * @返回值：
 *        SUCCESS - 成功
 *        ERROR   - 失败
 * @注意：该方法一般在main函数的while循环中调用
 */
ErrorStatus PAL_Button_Proc(PalButton_HandleTypeDef *Handle)
{
	BitAction currentState;
	uint64_t currentTime;
	
	currentState = (BitAction)Button_GetState(Handle);
	currentTime = PAL_GetTick();
	
	if(Handle->ChangePending)
	{
		if(currentTime < Handle->PendingTime + BUTTON_SETTLING_TIME)
		{
			// Do nothing
		}
		else
		{
			if(currentState != Handle->LastState)
			{
				Handle->LastState = currentState;
				if(currentState == Button_ActiveLevel(Handle))
				{
					if(Handle->Init.ButtonEventCallback != 0)
					{
						Handle->Init.ButtonEventCallback(BUTTON_EVT_PRESSED);
					}
				}
				else 
				{
					if(Handle->Init.ButtonEventCallback != 0)
					{
						Handle->Init.ButtonEventCallback(BUTTON_EVT_RELEASED);
					}
				}
				Handle->LastState = currentState;
			}
			else
			{
				// Do nothing
			}
			Handle->ChangePending = 0;
		}
	}
	else
	{
		if(currentState != Handle->LastState)
		{
			Handle->PendingTime = currentTime;
			Handle->ChangePending = 1;
		}
	}
	
	return SUCCESS;
}
