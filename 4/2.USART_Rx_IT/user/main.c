#include "stm32f10x.h"
#include "stm32f10x_pal.h"

static void UART1_Init(void);
static void LED_Init(void);
static char receive_data;

//USART�жϱ��
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
	//1.��ʼ��gpio����
	//USARTĬ���������òο����Ĳο��ֲ��жϵ�8.1.11�����GPIO����
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	// ����PA9ΪUART1 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//��Ϊʹ�õ������ŵ�uart���ܣ�����Ӧ��ѡ�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// ����PA10ΪUART1 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//2.����UART
	
	USART_InitTypeDef USART_InitStructure;

	// ʹ��UART1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// UART1����
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�������

	USART_Init(USART1, &USART_InitStructure);
	
	//3.�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//����RXNE��RXNE IE���ж�ʹ�ܣ��Ĵ�����ʹ����Բ����ж�
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	
	//4.ʹ��UART1��UART���ܿ��أ�������UART����Ҫ���ܿ��أ�
	USART_Cmd(USART1, ENABLE);

}

static void LED_Init(void) //led0 PB5 �͵�ƽ����
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
		receive_data = USART_ReceiveData(USART1); //��һ�����ȡRDR�Ĵ�������ȡ��RDR�Ĵ������
																							//RXNE�Ĵ������Ϊ0���жϱ�־λҲ�������
		if(receive_data == '0') //�͵�ƽ����
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		}
		if(receive_data == '1')
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		}	
	
	}



}
