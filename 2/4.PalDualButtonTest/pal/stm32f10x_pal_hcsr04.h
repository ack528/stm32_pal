/**
  ******************************************************************************
  * @file    stm32f10x_pal_hcsr04.h
  * @author  铁头山羊stm32工作组
  * @version V1.0.0
  * @date    2023年2月24日
  * @brief   hc-sr04驱动
  ******************************************************************************
  * @attention
  * Copyright (c) 2022 - 2023 东莞市明玉科技有限公司. 保留所有权力.
  ******************************************************************************
*/


#ifndef _STM32F10x_PAL_HCSR04_H_
#define _STM32F10x_PAL_HCSR04_H_

#include "stm32f10x.h"

typedef struct
{
	uint32_t Remap;
} PalHCSR04_AdvancedInitTypeDef;

typedef struct
{
	TIM_TypeDef *TIMx;
	GPIO_TypeDef *TrigGPIOPort;
	uint16_t TrigGPIOPin;
	PalHCSR04_AdvancedInitTypeDef Advanced;
}PalHCSR04_InitTypeDef;

typedef struct
{
	PalHCSR04_InitTypeDef Init;
	uint64_t LastTriggerTime;
} PalHCSR04_HandleTypeDef;

      void PAL_HCSR04_Init(PalHCSR04_HandleTypeDef *Handle);
      void PAL_HCSR04_Trigger(PalHCSR04_HandleTypeDef *Handle);
     float PAL_HCSR04_Read(PalHCSR04_HandleTypeDef *Handle);
FlagStatus PAL_HCSR04_IsBusy(PalHCSR04_HandleTypeDef *Handle);

#endif
