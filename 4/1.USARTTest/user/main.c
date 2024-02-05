#include "stm32f10x.h"
#include "stm32f10x_pal.h"

static void UART1_Init(void);
static void LED_Init(void);
static char receive_data;


int main(void)
{
	PAL_Init();
	UART1_Init();
	LED_Init();

	while(1)
	{
		
		
#if 0  //�궨���д�������if 0��ô������Ĵ��뱻���Բ������룬��˿�����Ϊ����Ƭ��ѡ��
		USART_SendData(USART1,0x5a);
		PAL_Delay(200);
		//���ն���ʾZ5A��ԭ��
		//ASCII����: ���������£�����ͨ�Ź��߻��ն˳���Ὣ���յ������ݽ���ΪASCII�ַ���
		//0x5A��ASCII�ж�Ӧ��д��ĸ'Z'�����Ե�������0x5Aʱ��
		//���ն˿���������ʾ������ֽڵ�ASCII�ַ����ͣ���'Z'��������������ʮ��������ʽ"5A"��
		
		
#elif 0		//������������

		uint16_t MY_Data[] = {0,1,2,3,4};
		uint16_t i = 0;
		for(i=0;i<5;i++)
		{
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
			USART_SendData(USART1,MY_Data[i]);
			PAL_Delay(200);//��Ϊ��whileѭ����������ʱһ�·�ֹ���͹���
		}
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);	
		
		//��������һ��Сʱbug
		//1.USART_GetFlagStatus()��Ҫ��USART_GetITStatus()Ū��
		//2.whileѭ��δ��;   ����whileĬ��ִ����һ�䣬����һֱ����00�������
		
		
#elif 0 //�����ַ���
		const char  * my_str = "hello world\n";
		uint16_t i = 0;
		for(i=0;i<strlen(my_str);i++)
		{
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
			USART_SendData(USART1,my_str[i]);
			PAL_Delay(200);//��Ϊ��whileѭ����������ʱһ�·�ֹ���͹���
		}
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		
#elif 1  //��Ƭ����������
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
		receive_data = USART_ReceiveData(USART1);
		if(receive_data == '0') //�͵�ƽ����
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		}
		if(receive_data == '1')
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		}
#endif
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

	// ʹ��UART1��UART���ܿ��أ�������UART����Ҫ���ܿ��أ�
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