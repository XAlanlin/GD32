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
	uint8_t 	run;                //调度标志，1：调度 0：挂起
	uint16_t  timCount;          //时间片计数值
	uint16_t  timRload;          //时间片重载值
	void (*pTaskFunCb)(void);     //函数指针变量，保存业务功能模块地址
}TaskComps_t;

static TaskComps_t g_taskComps[] = 
{
	{0 , 200 , 200 ,UsbTask},
	{0 , 5 , 5 ,HmiTask},
	/*添加业务功能模块*/
};

#define  TASK_NUM_MAX  ( sizeof(g_taskComps) / sizeof(g_taskComps[0]) )


static void Task_Handler(void)
{
	for(uint8_t i = 0; i < TASK_NUM_MAX; i++)
	{
			if(g_taskComps[i].run)
			{
				g_taskComps[i].run = 0;
				g_taskComps[i].pTaskFunCb();      // 执行调度业务功能模块
			}
	}
}

/**
***********************************************************
* @brief 在定时器中断服务函数中被间接调用，设置时间片标记，
         需要定时器1ms产生1次中断
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
