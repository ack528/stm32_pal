#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_button.h"
//任务：使用裸机多任务模型完成按键切换led闪灯速度

static uint32_t LED_Blink_Mode = 0; //0:慢闪 1:普通 2:快闪
static uint32_t LED_Blink_Time = 100;//当前led闪烁的时间间隔
static uint32_t LED_Blink_Time_Arr[] = {1000,500,100};//led闪烁的延时时间
PalButton_HandleTypeDef Handle;
static void LED_Init(void);
static void KEY_Init(void);
static void LED_Proc(void);
static void KEY_Proc(void);
static void LED_Blink_Slc(void);
int main(void)
{
	PAL_Init();	
//	1.初始化led灯和按键
	LED_Init();
	KEY_Init();
	while(1)
	{
		KEY_Proc();
		LED_Proc();
	}
}

static void LED_Init(void)//led_pin: PB5
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
static void KEY_Init(void) //使用pal库初始化key0
{
	Handle.Init.GPIO_Pin = GPIO_Pin_4;
	Handle.Init.GPIOx = GPIOE;
	Handle.Init.Button_Mode	= Button_Mode_IPU;
	Handle.Init.ButtonReleasedCallback = LED_Blink_Slc;
	Handle.Init.ButtonPressedCallback = 0;	
	PAL_Button_Init(&Handle);
}
static void LED_Blink_Slc(void)//led闪烁的模式切换
{
	LED_Blink_Mode++;
	LED_Blink_Time = LED_Blink_Time_Arr[LED_Blink_Mode%3];
}
static void KEY_Proc(void) //循环检测按键进程
{
	PAL_Button_Proc(&Handle);
}

static void LED_Proc(void)//led灯循环闪烁代码
{
	//使用上个系统时间-现在系统时间的方法获得led亮灭时间，从而避免使用delay函数，实现“多线程”
	static uint32_t mode = 0; //0；led灭   1：led亮  
	static uint64_t Last_Sys_Time = 0;
	if(mode == 0)
	{
		if(PAL_GetTick() - Last_Sys_Time >= LED_Blink_Time)
		{
			Last_Sys_Time = PAL_GetTick();
			GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);//led低电平亮
			mode = 1;
		}
	}
	
	if(mode == 1)
	{
		if(PAL_GetTick() - Last_Sys_Time >= LED_Blink_Time)
		{
			Last_Sys_Time = PAL_GetTick();
			GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);//led高电平灭
			mode = 0;
		}
	}	
}

