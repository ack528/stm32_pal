#include "bsp_queue.h"
//队列初始化

void Queue_Init(Queue_HandleTypeDef * Queue)
{
	Queue->Tail = 0;//将尾指针指向头即为完成初始化
}
void Enqueue(Queue_HandleTypeDef * Queue,uint8_t data)//不需要修改data原始内存上的数据，所以不用指针
{
	Queue->Data[Queue->Tail] = data; //把数据赋值给尾指针指向的内存
	Queue->Tail++;//尾指针++
}
ErrorStatus Dequeue(Queue_HandleTypeDef * Queue, uint8_t *pdata) //需要修改data原始内存上的数据，所以用指针针
{
	uint16_t i ;
	//出队
	//先要保证这个队列有数据，不能是空队列
	if(Queue->Tail<1)
		return ERROR;
	else
	{
		//把头部数据导出
		*pdata = Queue->Data[0];
		//所有数据往前移动一格
		for(i=0;i<Queue->Tail;i++)
		{
			Queue->Data[i] = Queue->Data[i+1];
		}
		//把尾指针往前移动一格
		Queue->Tail--;
		//返回成功
		return SUCCESS;
	}

}
