#include "bsp_queue.h"
//���г�ʼ��

void Queue_Init(Queue_HandleTypeDef * Queue)
{
	Queue->Tail = 0;//��βָ��ָ��ͷ��Ϊ��ɳ�ʼ��
}
void Enqueue(Queue_HandleTypeDef * Queue,uint8_t data)//����Ҫ�޸�dataԭʼ�ڴ��ϵ����ݣ����Բ���ָ��
{
	Queue->Data[Queue->Tail] = data; //�����ݸ�ֵ��βָ��ָ����ڴ�
	Queue->Tail++;//βָ��++
}
ErrorStatus Dequeue(Queue_HandleTypeDef * Queue, uint8_t *pdata) //��Ҫ�޸�dataԭʼ�ڴ��ϵ����ݣ�������ָ����
{
	uint16_t i ;
	//����
	//��Ҫ��֤������������ݣ������ǿն���
	if(Queue->Tail<1)
		return ERROR;
	else
	{
		//��ͷ�����ݵ���
		*pdata = Queue->Data[0];
		//����������ǰ�ƶ�һ��
		for(i=0;i<Queue->Tail;i++)
		{
			Queue->Data[i] = Queue->Data[i+1];
		}
		//��βָ����ǰ�ƶ�һ��
		Queue->Tail--;
		//���سɹ�
		return SUCCESS;
	}

}
