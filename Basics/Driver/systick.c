#include <stdint.h>
#include "gd32f30x.h"

uint64_t g_sysRunTime = 0;

static void (*g_pTaskSchduleFunc)(void);

/**
***********************************************************
* @brief 注册任务调度回调函数
* @param pFunc,传入回调函数地址
* @return 
***********************************************************
*/
void TaskScheduleCbReg(void (*pFunc)(void))
{
	g_pTaskSchduleFunc = pFunc;
}
/**
***********************************************************
* @brief systick初始化
* @param
* @return 
***********************************************************
*/
void SystickInit(void)
{
	/*1ms产生一次中断*/
	if(SysTick_Config(rcu_clock_freq_get(CK_AHB)/1000))
	{
		while(1);
	}
}
/**
***********************************************************
* @brief 定时器中断服务函数
* @param
* @return 
***********************************************************
*/
void SysTick_Handler(void)
{
	g_sysRunTime ++;
	g_pTaskSchduleFunc();

}
/**
***********************************************************
* @brief 获取系统运行时间
* @param
* @return 以1ms为单位
***********************************************************
*/
uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}
