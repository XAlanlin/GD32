#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "led_drv.h"
#include "usb_drv.h"

/**
***********************************************************************
包格式：帧头0  帧头1  数据长度  功能字   LED编号  亮/灭  异或校验数据
        0x55   0xAA    0x03      0x06     0x00     0x01      0xFB
***********************************************************************
*/
#define FRAME_HEAD_0        0x55  
#define FRAME_HEAD_1        0xAA
#define CTRL_DATA_LEN       3     //数据域长度
#define PACKET_DATA_LEN     (CTRL_DATA_LEN + 4)  //包长度
#define FUNC_DATA_IDX       3     //功能字数组下标
#define LED_CTRL_CODE       0x06  //功能字

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
* @brief 对数据进行异或运算
* @param data, 存储数组的首地址
* @param len, 要计算的元素的个数
* @return 异或运算结果
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
* @brief LED控制处理函数
* @param ctrlData，结构体指针，传入LED的编号和状态
* @return 
***********************************************************
*/
static void CtrlLed(LedCtrlInfo_t *ctrlData)
{
	ctrlData->ledState != 0 ? TurnOnLed(ctrlData->ledNo) : TurnOffLed(ctrlData->ledNo);
}

/**
***********************************************************
* @brief USB转串口任务处理函数
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
* @brief USB应用初始化函数
* @param
* @return 
***********************************************************
*/
void usbAppInit(void)
{
	regUsb2ComCb(ProcUartData);
}
