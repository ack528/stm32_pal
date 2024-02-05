#ifndef __APP_BLINKY_LED_H__
#define __APP_BLINKY_LED_H__

#include "stm32f10x.h"

void App_BlinkyLED_Init(void);
void App_BlinkyLED_Proc(void);
void App_BlinkyLED_SetOnTime(uint32_t Val);
void App_BlinkyLED_SetOffTime(uint32_t Val);
uint32_t App_BlinkyLED_GetOnTime(void);
uint32_t App_BlinkyLED_GetOffTime(void);

#endif
