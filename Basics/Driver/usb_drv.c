#include <stdint.h>
#include "gd32f30x.h"

typedef struct
{
	uint32_t uartNo;
	rcu_periph_enum rcuUart;
	rcu_periph_enum rcuGPIO;
	uint32_t gpio;
	uint32_t txPin;
	uint32_t rxPin;
}UartHwInfo;

static UartHwInfo g_uartHwInfo = {USART0,RCU_USART0,RCU_GPIOA,GPIOA,GPIO_PIN_9,GPIO_PIN_10};

static void UsbGPIOInit()
{

}

static void UsbUartInit(uint32_t Baud)
{

}

/**
***********************************************************
* @brief 串口初始化函数
* @param
* @return 
***********************************************************
*/
void UsbDrvInit(void)
{
	UsbGPIOInit();
	UsbUartInit(11520);
}
