#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "led_drv.h"
#include "usb_drv.h"

/**
***********************************************************************
����ʽ��֡ͷ0  ֡ͷ1  ���ݳ���  ������   LED���  ��/��  ���У������
        0x55   0xAA    0x03      0x06     0x00     0x01      0xFB
***********************************************************************
*/
#define FRAME_HEAD_0        0x55  
#define FRAME_HEAD_1        0xAA
#define CTRL_DATA_LEN       3     //�����򳤶�
#define PACKET_DATA_LEN     (CTRL_DATA_LEN + 4)  //������
#define FUNC_DATA_IDX       3     //�����������±�
#define LED_CTRL_CODE       0x06  //������

#define MAX_BUF_SIZE 20
static uint8_t g_rcvDataBuf[MAX_BUF_SIZE];
static bool g_pktRcvd = false;

typedef struct
{
	uint8_t ledNo;
	uint8_t ledState;
} LedCtrlInfo_t;

static void ProcUartData(uint8_t data)
{
	static uint8_t index = 0;
	g_rcvDataBuf[index++] = data;
	
	switch (index)
	{
		case 1:
			if (g_rcvDataBuf[0] != FRAME_HEAD_0)
			{
				index = 0;
			}
			break;
		case 2:
			if (g_rcvDataBuf[1] != FRAME_HEAD_1)
			{
				index = 0;
			}
			break;
		case PACKET_DATA_LEN:
			g_pktRcvd = true;
			index = 0;
			break;
		default:
			break;
	}
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
	if (!g_pktRcvd)
	{
		return;
	}
	g_pktRcvd = false;
	
	if (CalXorSum(g_rcvDataBuf, PACKET_DATA_LEN - 1) != g_rcvDataBuf[PACKET_DATA_LEN - 1])
	{
		return;
	}
	if (g_rcvDataBuf[FUNC_DATA_IDX] == LED_CTRL_CODE)
	{
		CtrlLed((LedCtrlInfo_t *)(&g_rcvDataBuf[FUNC_DATA_IDX + 1]));
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
}
