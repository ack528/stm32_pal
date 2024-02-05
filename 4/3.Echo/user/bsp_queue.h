#ifndef _BSP_QUEUE_H_
#define _BSP_QUEUE_H_
#include "stm32f10x.h"
typedef struct
{
	uint8_t Data[100];
	uint16_t Tail;
}Queue_HandleTypeDef;

void Queue_Init(Queue_HandleTypeDef * Queue);
void Enqueue(Queue_HandleTypeDef * Queue,uint8_t data);
ErrorStatus Dequeue(Queue_HandleTypeDef * Queue, uint8_t * pdata);

#endif
