#ifndef _KEY_DRV_H_
#define _KEY_DRV_H_

#include <stdint.h>

#define KEY1_SHORT_PRESS 0X01
#define KEY1_LONG_PRESS  0X81
#define KEY2_SHORT_PRESS 0X02
#define KEY2_LONG_PRESS  0X82
#define KEY3_SHORT_PRESS 0X03
#define KEY3_LONG_PRESS  0X83
#define KEY4_SHORT_PRESS 0X04
#define KEY4_LONG_PRESS  0X84
/**
***********************************************************
* @brief 按键硬件初始化
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void);
/**
***********************************************************
* @brief 获取按键码值
* @param
* @return 四个按键码值，短按0x01，0x02，0x03，0x04
           长按0x81，0x82，0x83，0x84，没有按下时为0
***********************************************************
*/
uint8_t GetKeyVal(void);

#endif
