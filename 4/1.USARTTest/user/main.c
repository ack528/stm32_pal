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
		
		
#if 0  //宏定义的写法，如果if 0那么这区间的代码被忽略不被编译，因此可以作为代码片段选择
		USART_SendData(USART1,0x5a);
		PAL_Delay(200);
		//接收端显示Z5A的原因
		//ASCII解释: 在许多情况下，串口通信工具或终端程序会将接收到的数据解释为ASCII字符。
		//0x5A在ASCII中对应大写字母'Z'。所以当您发送0x5A时，
		//接收端可能首先显示了这个字节的ASCII字符解释，即'Z'，紧接着是它的十六进制形式"5A"。
		
		
#elif 0		//连续发送数组

		uint16_t MY_Data[] = {0,1,2,3,4};
		uint16_t i = 0;
		for(i=0;i<5;i++)
		{
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
			USART_SendData(USART1,MY_Data[i]);
			PAL_Delay(200);//因为在while循环里所以延时一下防止发送过快
		}
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);	
		
		//这里修了一个小时bug
		//1.USART_GetFlagStatus()不要与USART_GetITStatus()弄混
		//2.while循环未加;   导致while默认执行下一句，所以一直发送00这个数据
		
		
#elif 0 //发送字符串
		const char  * my_str = "hello world\n";
		uint16_t i = 0;
		for(i=0;i<strlen(my_str);i++)
		{
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
			USART_SendData(USART1,my_str[i]);
			PAL_Delay(200);//因为在while循环里所以延时一下防止发送过快
		}
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		
#elif 1  //单片机接收数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
		receive_data = USART_ReceiveData(USART1);
		if(receive_data == '0') //低电平点亮
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

	// 使能UART1（UART有总开关，配置完UART后需要打开总开关）
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