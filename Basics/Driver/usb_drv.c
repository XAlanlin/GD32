#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "led_drv.h"

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

static void Usb2ComGpioInit(void)
{
	rcu_periph_clock_enable(g_uartHwInfo.rcuGpio);
	gpio_init(g_uartHwInfo.gpio, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, g_uartHwInfo.txPin);
	gpio_init(g_uartHwInfo.gpio, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, g_uartHwInfo.rxPin);
}

static void Usb2ComUartInit(uint32_t baudRate)
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
	Usb2ComGpioInit();
	Usb2ComUartInit(115200);
}

/**
***********************************************************************
����ʽ��֡ͷ0  ֡ͷ1  ���ݳ���  ������   LED���  ��/��  ���У������
        0x55   0xAA    0x03      0x06     0x00     0x01      0xFB
***********************************************************************
*/
#define FRAME_HEAD_0        0x55  
#define FRAME_HEAD_1        0xAA
#define CTRL_DATA_LEN       3     //�����򳤶�
#define PACKET_DATA_LEN     (CTRL_DATA_LEN + 4)  //������
#define FUNC_DATA_IDX       3     //�����������±�
#define LED_CTRL_CODE       0x06  //������

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
* @brief �����ݽ����������
* @param data, �洢������׵�ַ
* @param len, Ҫ�����Ԫ�صĸ���
* @return ���������
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
* @brief LED���ƴ�����
* @param ctrlData���ṹ��ָ�룬����LED�ı�ź�״̬
* @return 
***********************************************************
*/
static void CtrlLed(LedCtrlInfo_t *ctrlData)
{
	ctrlData->ledState != 0 ? TurnOnLed(ctrlData->ledNo) : TurnOffLed(ctrlData->ledNo);
}

/**
***********************************************************
* @brief USBת������������
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
		ProcUartData(uData);
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
