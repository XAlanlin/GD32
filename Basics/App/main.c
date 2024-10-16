#include <stdint.h>
#include <stdio.h>
#include "led_drv.h"
#include "key_drv.h"
#include "systick.h"
#include "usb_drv.h"
#include "usb_APP.h"
#include "Hmi_App.h"

typedef struct
{
	uint8_t 	run;                //���ȱ�־��1������ 0������
	uint16_t  timCount;          //ʱ��Ƭ����ֵ
	uint16_t  timRload;          //ʱ��Ƭ����ֵ
	void (*pTaskFunCb)(void);     //����ָ�����������ҵ����ģ���ַ
}TaskComps_t;

static TaskComps_t g_taskComps[] = 
{
	{0 , 200 , 200 ,UsbTask},
	{0 , 5 , 5 ,HmiTask},
	/*���ҵ����ģ��*/
};

#define  TASK_NUM_MAX  ( sizeof(g_taskComps) / sizeof(g_taskComps[0]) )


static void Task_Handler(void)
{
	for(uint8_t i = 0; i < TASK_NUM_MAX; i++)
	{
			if(g_taskComps[i].run)
			{
				g_taskComps[i].run = 0;
				g_taskComps[i].pTaskFunCb();      // ִ�е���ҵ����ģ��
			}
	}
}

/**
***********************************************************
* @brief �ڶ�ʱ���жϷ������б���ӵ��ã�����ʱ��Ƭ��ǣ�
         ��Ҫ��ʱ��1ms����1���ж�
* @param
* @return 
***********************************************************
*/
static void TaskScheduleCb(void)
{
	for (uint8_t i = 0; i < TASK_NUM_MAX; i++)
	{
		if (g_taskComps[i].timCount)
		{
			g_taskComps[i].timCount--;
			if (g_taskComps[i].timCount == 0)
			{
				g_taskComps[i].run = 1;
				g_taskComps[i].timCount = g_taskComps[i].timRload;
			}
		}
	}
}
void DriverInit(void)
{
	SystickInit();
	LedDrvInit();
	KeyDrvInit();
	UsbDrvInit();
}
void AppInit(void)
{
	usbAppInit();
	TaskScheduleCbReg(TaskScheduleCb);
}

int main(void)
{	
  DriverInit();
  AppInit();
	while (1)
	{
		Task_Handler();
	}
}
