#include <stdint.h>
#include "queue.h"

/**
****************************************************************
* @brief   ��ʼ�������������У�ÿ����������ִ�иú�������ʹ��
* @param   queue, ���б���ָ��
* @param   buffer, ���л�������ַ
* @param   size, ���л���������
* @return 
****************************************************************
*/
void QueueInit(QueueType_t *queue, uint8_t *buffer, uint32_t size)
{
    queue->buffer = buffer;
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
}

/**
****************************************************************
* @brief   ѹ�����ݵ�������
* @param   queue, ���б���ָ��
* @param   data, ��ѹ����е�����
* @return  ѹ������Ƿ�ɹ�
****************************************************************
*/
QueueStatus_t QueuePush(QueueType_t *queue, uint8_t data)
{
    uint32_t index = (queue->tail + 1) % queue->size;

    if (index == queue->head)
    {
        return QUEUE_OVERLOAD;
    }
    queue->buffer[queue->tail] = data;
    queue->tail = index;
    return QUEUE_OK;
}

/**
****************************************************************
* @brief   �Ӷ����е�������
* @param   queue, ���б���ָ��
* @param   pdata, ���������е����ݻ����ַ
* @return  ���������Ƿ�ɹ�
****************************************************************
*/
QueueStatus_t QueuePop(QueueType_t *queue, uint8_t *pdata)
{
    if(queue->head == queue->tail)
    {
        return QUEUE_EMPTY;
    }

    *pdata = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    return QUEUE_OK;
}

/**
****************************************************************
* @brief   ѹ��һ�����ݵ�������
* @param   queue, ���б���ָ��
* @param   pArray, ��ѹ����е������ַ
* @param   len, ��ѹ����е�Ԫ�ظ���
* @return  ʵ��ѹ�뵽���е�Ԫ�ظ���
****************************************************************
*/
uint32_t QueuePushArray(QueueType_t *queue, uint8_t *pArray, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if(QueuePush(queue, pArray[i]) == QUEUE_OVERLOAD)
        {
            break;
        }
    }
    return i;
}

/**
****************************************************************
* @brief   �Ӷ����е���һ������
* @param   queue, ���б���ָ��
* @param   pArray, ���������е����ݻ����ַ
* @param   len, ���������е����ݵ���󳤶�
* @return  ʵ�ʵ������ݵ�����
****************************************************************
*/
uint32_t QueuePopArray(QueueType_t *queue, uint8_t *pArray, uint32_t len)
{
    uint32_t i;
    for(i = 0; i < len; i++)
    {
        if (QueuePop(queue, &pArray[i]) == QUEUE_EMPTY)
        {
            break;
        }
    }
    return i;
}

/**
****************************************************************
* @brief   ��ȡ���������ݵĸ���
* @param   queue, ���б���ָ��
* @return  ���������ݵĸ���
****************************************************************
*/
uint32_t QueueCount(QueueType_t *queue)
{
	if (queue->head <= queue->tail)
	{
		return queue->tail - queue->head;
	}
	
	return queue->size + queue->tail - queue->head;
}
