#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "app_cmd.h"
#include "app_blinky_led.h"

int main(void)
{
	PAL_Init();
	App_BlinkyLED_Init();
	App_Cmd_Init();
	
	while(1)
	{
		App_BlinkyLED_Proc();
		App_Cmd_Proc();
	}
}
