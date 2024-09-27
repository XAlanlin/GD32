#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"


typedef struct
{
	uint32_t uartNo;
	rcu_periph_enum rcuUart;
	rcu_periph_enum rcuGpio;
	uint32_t gpio;
	uint32_t txPin;
	uint32_t rxPin;
	uint8_t irq;
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {USART0, RCU_USART0, RCU_GPIOA, GPIOA, GPIO_PIN_9, GPIO_PIN_10, USART0_IRQn};

static void UsbGpioInit(void)
{
	rcu_periph_clock_enable(g_uartHwInfo.rcuGpio);
	gpio_init(g_uartHwInfo.gpio, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, g_uartHwInfo.txPin);
	gpio_init(g_uartHwInfo.gpio, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, g_uartHwInfo.rxPin);
}

static void UsbUartInit(uint32_t baudRate)
{
	/* ʹ��UARTʱ�ӣ�*/
	rcu_periph_clock_enable(g_uartHwInfo.rcuUart);
	/* ��λUART��*/
	usart_deinit (g_uartHwInfo.uartNo);
	/* ͨ��USART_CTL0�Ĵ�����WL�����ֳ���*/ 
	//usart_word_length_set(g_uartHwInfo.uartNo, USART_WL_8BIT);
	/* ͨ��USART_CTL0�Ĵ�����PCEN����У��λ��*/ 
	//usart_parity_config(g_uartHwInfo.uartNo, USART_PM_NONE);
	/* ��USART_CTL1�Ĵ�����дSTB[1:0]λ������ֹͣλ�ĳ��ȣ�*/ 
	//usart_stop_bit_set(g_uartHwInfo.uartNo, USART_STB_1BIT);
	/* ��USART_BAUD�Ĵ��������ò����ʣ�*/ 
	usart_baudrate_set(g_uartHwInfo.uartNo, baudRate);
	/* ��USART_CTL0�Ĵ���������TENλ��ʹ�ܷ��͹��ܣ�*/
	usart_transmit_config(g_uartHwInfo.uartNo, USART_TRANSMIT_ENABLE);
	/* ��USART_CTL0�Ĵ���������TENλ��ʹ�ܽ��չ��ܣ�*/
	usart_receive_config(g_uartHwInfo.uartNo, USART_RECEIVE_ENABLE);
	/* ʹ�ܴ��ڽ����жϣ�*/
	usart_interrupt_enable(g_uartHwInfo.uartNo, USART_INT_RBNE);
	/* ʹ�ܴ����жϣ�*/
	nvic_irq_enable(g_uartHwInfo.irq, 0, 0);
	/* ��USART_CTL0�Ĵ�������λUENλ��ʹ��UART��*/ 
	usart_enable(g_uartHwInfo.uartNo);
}

/**
***********************************************************
* @brief USBת����Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void UsbDrvInit(void)
{
	UsbGpioInit();
	UsbUartInit(115200);
}

static void (*pProcUartData)(uint8_t data);  //ָ�뺯��

/**
***********************************************************
* @brief ע��ص�������
* @param pFunc�ص���ַ
* @return 
***********************************************************
*/
void regUsb2ComCb (void(*pFunc)(uint8_t data))
{
	pProcUartData = pFunc;
}

/**
***********************************************************
* @brief ����0�жϷ�����
* @param
* @return 
***********************************************************
*/
void USART0_IRQHandler(void)
{
	if (usart_interrupt_flag_get(g_uartHwInfo.uartNo, USART_INT_FLAG_RBNE) != RESET)
	{
		usart_interrupt_flag_clear(g_uartHwInfo.uartNo, USART_INT_FLAG_RBNE);
		uint8_t uData = (uint8_t)usart_data_receive(g_uartHwInfo.uartNo);
		pProcUartData(uData);
	}
}

/**
***********************************************************
* @brief printf����Ĭ�ϴ�ӡ�������ʾ�������Ҫ��������ڣ�
		 ��������ʵ��fputc�����������ָ�򴮿ڣ���Ϊ�ض���
* @param
* @return 
***********************************************************
*/
int fputc(int ch, FILE *f)
{
	usart_data_transmit(g_uartHwInfo.uartNo, (uint8_t)ch);
	while (RESET == usart_flag_get(g_uartHwInfo.uartNo, USART_FLAG_TBE));
	return ch;
}
