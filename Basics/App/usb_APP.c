#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "led_drv.h"
#include "usb_drv.h"
#include "queue.h"

/**
***********************************************************************
����ʽ��֡ͷ0  ֡ͷ1  ���ݳ���  ������   LED���  ��/��  ���У������
        0x55   0xAA    0x03      0x06     0x00     0x01      0xFB
***********************************************************************
*/
#define FRAME_HEAD_0           0x55  
#define FRAME_HEAD_1           0xAA
#define PACKET_DATA_LEN_MIN    7                  //��С������
#define PACKET_DATA_LEN_MAX    11                 //��������
#define FUNC_DATA_IDX          3                  //�����������±�
#define LED_CTRL_CODE          0x06  
#define MAX_BUF_SIZE 77

static uint8_t g_rcvDataBuf[MAX_BUF_SIZE];
static QueueType_t g_rcvQueue;

typedef struct
{
	uint8_t ledNo;
	uint8_t ledState;
} LedCtrlInfo_t;

static void ProcUartData(uint8_t data)
{
	QueuePush(&g_rcvQueue, data);
}

/**
***********************************************************
* @brief �����ݽ����������
* @param data, �洢������׵�ַ
* @param len, Ҫ�����Ԫ�صĸ���
* @return ���������
***********************************************************
*/
static uint8_t CalXorSum(const uint8_t *data, uint32_t len)
{
	uint8_t xorSum = 0;
	for (uint32_t i = 0; i < len; i++)
	{
		xorSum ^= data[i];
	}
	return xorSum;
}

/**
***********************************************************
* @brief LED���ƴ�����
* @param ctrlData���ṹ��ָ�룬����LED�ı�ź�״̬
* @return 
***********************************************************
*/
static void CtrlLed(LedCtrlInfo_t *ctrlData)
{
	ctrlData->ledState != 0 ? TurnOnLed(ctrlData->ledNo) : TurnOffLed(ctrlData->ledNo);
}

/**
***********************************************************
* @brief USBת������������
* @param
* @return 
***********************************************************
*/
void UsbTask(void)
{
	uint8_t readBuf[PACKET_DATA_LEN_MAX] = {0};
	
	while (QueuePop(&g_rcvQueue, &readBuf[0]) == QUEUE_OK)
	{
		if (readBuf[0] != FRAME_HEAD_0)
		{
			continue;
		}
		if ((QueuePop(&g_rcvQueue, &readBuf[1]) == QUEUE_EMPTY) || (readBuf[1] != FRAME_HEAD_1))
		{
			continue;
		}
		if ((QueuePop(&g_rcvQueue, &readBuf[2]) == QUEUE_EMPTY) || (readBuf[2] > PACKET_DATA_LEN_MAX))
	    {
			continue;
		}
		if (QueuePopArray(&g_rcvQueue, &readBuf[3], readBuf[2] + 1) != readBuf[2] + 1)
		{
			continue;
		}
		if (CalXorSum(readBuf, readBuf[2] + 3) != readBuf[readBuf[2] + 3])
		{
			continue;
		}
		if (readBuf[FUNC_DATA_IDX] == LED_CTRL_CODE)
		{
			CtrlLed((LedCtrlInfo_t *)(&readBuf[FUNC_DATA_IDX + 1]));
		}
		
		for (uint32_t i = 0; i < readBuf[2] + 4; i++)
		{
			printf("%02x ", readBuf[i]);
		}
		printf("\n");
	}
}

/***********************************************************
* @brief USBӦ�ó�ʼ������
* @param
* @return 
***********************************************************
*/
void usbAppInit(void)
{
	regUsb2ComCb(ProcUartData);
	QueueInit(&g_rcvQueue, g_rcvDataBuf,MAX_BUF_SIZE);
}
