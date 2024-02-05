//******************************�ܽ�************************************//
//ͨ�����ڿ�����ѧϰ���˵ڶ���ʵ���������ķ���
//UART1_Proc()�����������ǽ����������ݲ����ͳ�ȥ����ǰ����һ�������������
//��һ��ѭ����һֱ�ȴ���������ɣ��������Ǹ�Ϊʹ��ָ��ķ���������һ�����
//����ֻ����һ�����ݣ�Ȼ��ָ�루��־λ�������ƶ�һ���´ν���ú����ӱ�־
//λ��ʼ�洢��ͬʱÿ�ν���ú���ʱ�ж�һ�£���������û�С�

//����Ϊ����д�Ļ��ǲ����ã��������ݻ�������forѭ������ʵҲ�ǿ���ʹ�ø÷���
//�������ݵ�

//�ο�����1������1bit���� 100us  ����һ�д��� 0.1us
//�ο�����2����һ�����������ģ�ͣ������Ҫdelay�����Ľ��������
//����Ҫ�õ�delay()�����Ĵ����У����ǿ����Ȼ�ȡϵͳʱ�䲢����һ�������У�
//����������ŵ���ѭ���У�ÿ�ν���ú��������ж�һ�ε�ǰϵͳʱ��-�����е�ϵͳ
//ʱ���Ƿ��������Ҫ����ʱʱ�䣬�����ͱ�����ʹ��delay����
//**********************************************************************//
#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "bsp_queue.h"
#include <string.h>

static void UART1_Init(void);
static void UART1_Proc(void);
static void USART_Sendstring(const char *Str);

static char receive_data;
Queue_HandleTypeDef Queue; //��������

//USART�жϱ��
int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж����ȼ�����
	PAL_Init();
	UART1_Init();
	Queue_Init(&Queue); //��ʼ������
	

	while(1)
	{
		UART1_Proc();
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


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		receive_data = USART_ReceiveData(USART1); //��һ�����ȡRDR�Ĵ�������ȡ��RDR�Ĵ������
																							//RXNE�Ĵ������Ϊ0���жϱ�־λҲ�������
		Enqueue(&Queue,receive_data);
	}
}


ErrorStatus state_flag;
uint8_t temp_data; //���������ȡ��������ʱ����
uint8_t queue_data[100];//����ȡ����������
uint8_t pointer = 0;//ָ��

static void UART1_Proc(void)
{
	//ȷ��ȡ����ʱ�򲻻�������ݣ���������ʱ�����ν������ݵ��жϣ�
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	//��ȡ���������ݣ��ֽ�״̬��Ϣ��ֵ��state_flag
	state_flag = Dequeue(&Queue, &temp_data);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//�ռ��������ݣ�������Ƿ�ȡ������
	if(state_flag == SUCCESS)
	{
		queue_data[pointer] = temp_data;
		pointer++;
		
		//�ж��Ƿ��ռ���һ��������
		if(pointer>2&&queue_data[pointer-2]=='\r'&&queue_data[pointer-1]=='\n')
		{
			queue_data[pointer] = '\0';
			USART_Sendstring((const char *)queue_data); //��������
			pointer = 0;//�´��յ�����ʱ��ͷ��ʼ��
		}
	}
}

static void USART_Sendstring(const char *Str)
{
	uint8_t i;
	for(i=0;i<strlen(Str);i++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//��ֹ���ݻ�û���ͳ�ȥ������TDR�Ĵ���д����
		USART_SendData(USART1,Str[i]);
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
}
