#include <stdint.h>
#include "gd32f30x.h"

uint64_t g_sysRunTime = 0;

/**
***********************************************************
* @brief systick��ʼ��
* @param
* @return 
***********************************************************
*/
void SystickInit(void)
{
	/*1ms����һ���ж�*/
	if(SysTick_Config(rcu_clock_freq_get(CK_AHB)/1000))
	{
		while(1);
	}
}
/**
***********************************************************
* @brief ��ʱ���жϷ�����
* @param
* @return 
***********************************************************
*/
void SysTick_Handler(void)
{
	g_sysRunTime ++;

}
/**
***********************************************************
* @brief ��ȡϵͳ����ʱ��
* @param
* @return ��1msΪ��λ
***********************************************************
*/
uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}
