#include <stdint.h>
#include "gd32f30x.h"
#include "systick.h"

typedef struct
{
	rcu_periph_enum rcu;
	uint32_t gpio;
	uint32_t pin;
}Key_GPIO_t;

static Key_GPIO_t g_gpioList[] =
{
	{RCU_GPIOA, GPIOA, GPIO_PIN_0}, //key1
	{RCU_GPIOG, GPIOG, GPIO_PIN_13},//key2
	{RCU_GPIOG, GPIOG, GPIO_PIN_14},//key3
	{RCU_GPIOG, GPIOG, GPIO_PIN_15} //key4
};

#define KEY_NUM_MAX (sizeof(g_gpioList) / sizeof(g_gpioList[0]))
	
/**
***********************************************************
* @brief 按键初始化
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void)
{
	for (uint8_t i=0; i< KEY_NUM_MAX; i++)
	{
		rcu_periph_clock_enable(g_gpioList[i].rcu);
		gpio_init(g_gpioList[i].gpio, GPIO_MODE_IN_FLOATING, 
	            GPIO_OSPEED_2MHZ, g_gpioList[i].pin);
	}
}
typedef enum
{
	KEY_RELEASE = 0,//释放松开
	KEY_CONFIRM,    //消抖确认
	KEY_SHORTPRESS, //短按
	KEY_LONGPRESS   //长按
} KEY_STATE;

#define CONFIRM_TIME 10 //按键消抖时间窗 10ms
#define LONGPRESS_TIME 1000//长按时间窗 1000ms
typedef struct
{
	KEY_STATE keyState;
	uint64_t prvSysTime;
}Key_Info_t;

static Key_Info_t g_keyInfo[KEY_NUM_MAX];
/**
***********************************************************
* @brief 按键扫描
* @param
* @return 
***********************************************************
*/

static uint8_t KeyScan(uint8_t keyIndex)
{
	uint64_t curSysTime;
	uint8_t keyPress;
	
	keyPress = gpio_input_bit_get(g_gpioList[keyIndex].gpio, g_gpioList[keyIndex].pin);

	switch(g_keyInfo[keyIndex].keyState)
	{
		case KEY_RELEASE:
			if(!keyPress)
			{
				g_keyInfo[keyIndex].keyState = KEY_CONFIRM;
				g_keyInfo[keyIndex].prvSysTime = GetSysRunTime();
			}
			break;
		case KEY_CONFIRM:
			if(!keyPress)
			{
				curSysTime = GetSysRunTime();
				if(curSysTime - g_keyInfo[keyIndex].prvSysTime > CONFIRM_TIME )
				{
					g_keyInfo[keyIndex].keyState = KEY_SHORTPRESS;
				}
			}
			else
			{
				g_keyInfo[keyIndex].keyState = KEY_RELEASE;
			}
			break;
		case KEY_SHORTPRESS:
			if (keyPress)
			{
				g_keyInfo[keyIndex].keyState =  KEY_RELEASE;// 第一次出错,将RELEASE写为了SHORTPRESS,只能按一个按键
				return (keyIndex +1);//返回短按按键码值
			}
			else
			{
				curSysTime = GetSysRunTime();
				if (curSysTime - g_keyInfo[keyIndex].prvSysTime > LONGPRESS_TIME)
				{
					g_keyInfo[keyIndex].keyState = KEY_LONGPRESS;
				}
			}
		case KEY_LONGPRESS:
			if (keyPress)
			{
				g_keyInfo[keyIndex].keyState = KEY_RELEASE;
				return (keyIndex +0x81);//返回长按按键码值
			}
			break;
		default:
			g_keyInfo[keyIndex].keyState = KEY_RELEASE;
			break;
	}
	return 0;
}
/**
***********************************************************
* @brief 获取按键码值
* @param
* @return 四个按键码值，短按0x01，0x02，0x03，0x04
           长按0x81，0x82，0x83，0x84，没有按下时为0
***********************************************************
*/
uint8_t GetKeyVal(void)
{
	uint8_t res = 0;
	for (uint8_t i=0; i< KEY_NUM_MAX; i++)
	{
		res = KeyScan(i);
		if(res != 0)
		{
			break;
		}
	}
	return res;
}
