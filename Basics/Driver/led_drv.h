#ifndef LED_DRV_H_
#define LED_DRV_H_

#include <stdint.h>
#include "gd32f30x.h"

#define LED1 0
#define LED2 1
#define LED3 2

/**
***********************************************************
* @brief LEDӲ����ʼ��
* @param
* @return 
***********************************************************
*/
void LedDrvInit(void);

/**
***********************************************************
* @brief ����LED
* @param ledNo��LED��ţ�0~2
* @return 
***********************************************************
*/
void TurnOnLed(uint8_t ledNo);

/**
***********************************************************
* @brief Ϩ��LED
* @param ledNo��LED��ţ�0~2
* @return 
***********************************************************
*/
void TurnOffLed(uint8_t ledNo);

/**
***********************************************************
* @brief LED״̬ȡ��
* @param ledNo��LED��ţ�0~2
* @return 
***********************************************************
*/
void ToggleLed(uint8_t ledNo);

#endif
