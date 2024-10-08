#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>

typedef struct
{
    uint32_t head;        //�����±ָ꣬���ͷ
    uint32_t tail;        //�����±ָ꣬���β
    uint32_t size;        //���л��泤�ȣ���ʼ��ʱ��ֵ��
    uint8_t *buffer;      //���л������飨��ʼ��ʱ��ֵ��
} QueueType_t;

typedef enum
{
	QUEUE_OK = 0,       //��������
	QUEUE_ERROR,        //���д���
	QUEUE_OVERLOAD,     //��������
	QUEUE_EMPTY         //�����ѿ�
} QueueStatus_t;

/**
****************************************************************
* @brief   ��ʼ�������������У�ÿ����������ִ�иú�������ʹ��
* @param   queue, ���б���ָ��
* @param   buffer, ���л�������ַ
* @param   size, ���л���������
* @return 
****************************************************************
*/
void QueueInit(QueueType_t *queue, uint8_t *buffer, uint32_t size);

/**
****************************************************************
* @brief   ѹ�����ݵ�������
* @param   queue, ���б���ָ��
* @param   data, ��ѹ����е�����
* @return  ѹ������Ƿ�ɹ�
****************************************************************
*/
QueueStatus_t QueuePush(QueueType_t *queue, uint8_t data);

/**
****************************************************************
* @brief   �Ӷ����е�������
* @param   queue, ���б���ָ��
* @param   pdata, ���������е����ݻ����ַ
* @return  ���������Ƿ�ɹ�
****************************************************************
*/
QueueStatus_t QueuePop(QueueType_t *queue, uint8_t *pdata);

/**
****************************************************************
* @brief   ѹ��һ�����ݵ�������
* @param   queue, ���б���ָ��
* @param   pArray, ��ѹ����е������ַ
* @param   len, ��ѹ����е�Ԫ�ظ���
* @return  ʵ��ѹ�뵽���е�Ԫ�ظ���
****************************************************************
*/
uint32_t QueuePushArray(QueueType_t *queue, uint8_t *pArray, uint32_t len);

/**
****************************************************************
* @brief   �Ӷ����е���һ������
* @param   queue, ���б���ָ��
* @param   pArray, ���������е����ݻ����ַ
* @param   len, ���������е����ݵ���󳤶�
* @return  ʵ�ʵ������ݵ�����
****************************************************************
*/
uint32_t QueuePopArray(QueueType_t *queue, uint8_t *pArray, uint32_t len);

/**
****************************************************************
* @brief   ��ȡ���������ݵĸ���
* @param   queue, ���б���ָ��
* @return  ���������ݵĸ���
****************************************************************
*/
uint32_t QueueCount(QueueType_t *queue);

#endif
