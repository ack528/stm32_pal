//******************************总结************************************//
//通过本节课我们学习到了第二个实现裸机任务的方法
//UART1_Proc()函数的作用是接受所有数据并发送出去，以前我们一般在这个函数里
//放一个循环，一直等待他接受完成，现在我们改为使用指针的方法，运行一次这个
//函数只接受一个数据，然后指针（标志位）往后移动一格，下次进入该函数从标志
//位开始存储。同时每次进入该函数时判断一下，接收完了没有。

//我认为作者写的还是不够好，发送数据还是用了for循环，其实也是可以使用该方法
//发送数据的

//参考数据1：接收1bit数据 100us  处理一行代码 0.1us
//参考数据2：第一种裸机多任务模型（针对需要delay函数的解决方法）
//在需要用到delay()函数的代码中，我们可以先获取系统时间并存在一个变量中，
//将这个函数放到主循环中，每次进入该函数，就判断一次当前系统时间-变量中的系统
//时间是否大于你需要的延时时间，这样就避免了使用delay函数
//**********************************************************************//
#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "bsp_queue.h"
#include <string.h>

static void UART1_Init(void);
static void UART1_Proc(void);
static void USART_Sendstring(const char *Str);

static char receive_data;
Queue_HandleTypeDef Queue; //创建队列

//USART中断编程
int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组
	PAL_Init();
	UART1_Init();
	Queue_Init(&Queue); //初始化队列
	

	while(1)
	{
		UART1_Proc();
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


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		receive_data = USART_ReceiveData(USART1); //这一步会读取RDR寄存器，读取后，RDR寄存器清空
																							//RXNE寄存器会变为0，中断标志位也被清楚了
		Enqueue(&Queue,receive_data);
	}
}


ErrorStatus state_flag;
uint8_t temp_data; //用来存出列取出来的临时数据
uint8_t queue_data[100];//队列取出来的数据
uint8_t pointer = 0;//指针

static void UART1_Proc(void)
{
	//确保取数据时候不会接受数据（发送数据时候，屏蔽接受数据的中断）
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	//即取出来了数据，又将状态信息赋值给state_flag
	state_flag = Dequeue(&Queue, &temp_data);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//收集所有数据，并检测是否取完数据
	if(state_flag == SUCCESS)
	{
		queue_data[pointer] = temp_data;
		pointer++;
		
		//判断是否收集完一整行数据
		if(pointer>2&&queue_data[pointer-2]=='\r'&&queue_data[pointer-1]=='\n')
		{
			queue_data[pointer] = '\0';
			USART_Sendstring((const char *)queue_data); //发送数据
			pointer = 0;//下次收到数据时从头开始存
		}
	}
}

static void USART_Sendstring(const char *Str)
{
	uint8_t i;
	for(i=0;i<strlen(Str);i++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//防止数据还没发送出去，就向TDR寄存器写数据
		USART_SendData(USART1,Str[i]);
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
}
