#include "stm32f10x.h"
#include "stm32f10x_pal.h"

static void UART1_Init(void);
static void LED_Init(void);
static char receive_data;

//USART中断编程
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	PAL_Init();
	UART1_Init();
	LED_Init();

	while(1)
	{

	}
}



static void UART1_Init(void)
{
	//1.初始化gpio引脚
	//USART默认引脚配置参考中文参考手册中断的8.1.11外设的GPIO配置
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	// 配置PA9为UART1 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//因为使用的是引脚的uart功能，所以应该选择复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 配置PA10为UART1 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//2.配置UART
	
	USART_InitTypeDef USART_InitStructure;

	// 使能UART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// UART1配置
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发都开启

	USART_Init(USART1, &USART_InitStructure);
	
	//3.配置中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//配置RXNE和RXNE IE（中断使能）寄存器，使其可以产生中断
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	
	//4.使能UART1（UART有总开关，配置完UART后需要打开总开关）
	USART_Cmd(USART1, ENABLE);

}

static void LED_Init(void) //led0 PB5 低电平点亮
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

}
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		receive_data = USART_ReceiveData(USART1); //这一步会读取RDR寄存器，读取后，RDR寄存器清空
																							//RXNE寄存器会变为0，中断标志位也被清楚了
		if(receive_data == '0') //低电平点亮
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		}
		if(receive_data == '1')
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		}	
	
	}



}
