#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_button.h"
//����ʹ�����������ģ����ɰ����л�led�����ٶ�

static uint32_t LED_Blink_Mode = 0; //0:���� 1:��ͨ 2:����
static uint32_t LED_Blink_Time = 100;//��ǰled��˸��ʱ����
static uint32_t LED_Blink_Time_Arr[] = {1000,500,100};//led��˸����ʱʱ��
PalButton_HandleTypeDef Handle;
static void LED_Init(void);
static void KEY_Init(void);
static void LED_Proc(void);
static void KEY_Proc(void);
static void LED_Blink_Slc(void);
int main(void)
{
	PAL_Init();	
//	1.��ʼ��led�ƺͰ���
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
static void KEY_Init(void) //ʹ��pal���ʼ��key0
{
	Handle.Init.GPIO_Pin = GPIO_Pin_4;
	Handle.Init.GPIOx = GPIOE;
	Handle.Init.Button_Mode	= Button_Mode_IPU;
	Handle.Init.ButtonReleasedCallback = LED_Blink_Slc;
	Handle.Init.ButtonPressedCallback = 0;	
	PAL_Button_Init(&Handle);
}
static void LED_Blink_Slc(void)//led��˸��ģʽ�л�
{
	LED_Blink_Mode++;
	LED_Blink_Time = LED_Blink_Time_Arr[LED_Blink_Mode%3];
}
static void KEY_Proc(void) //ѭ����ⰴ������
{
	PAL_Button_Proc(&Handle);
}

static void LED_Proc(void)//led��ѭ����˸����
{
	//ʹ���ϸ�ϵͳʱ��-����ϵͳʱ��ķ������led����ʱ�䣬�Ӷ�����ʹ��delay������ʵ�֡����̡߳�
	static uint32_t mode = 0; //0��led��   1��led��  
	static uint64_t Last_Sys_Time = 0;
	if(mode == 0)
	{
		if(PAL_GetTick() - Last_Sys_Time >= LED_Blink_Time)
		{
			Last_Sys_Time = PAL_GetTick();
			GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);//led�͵�ƽ��
			mode = 1;
		}
	}
	
	if(mode == 1)
	{
		if(PAL_GetTick() - Last_Sys_Time >= LED_Blink_Time)
		{
			Last_Sys_Time = PAL_GetTick();
			GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);//led�ߵ�ƽ��
			mode = 0;
		}
	}	
}

