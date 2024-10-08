#include <stdint.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "led_drv.h"
#include "key_drv.h"
#include "systick.h"
#include "usb_drv.h"
#include "usbAPP.h"

void driverInit(void)
{
	SystickInit();
	LedDrvInit();
	KeyDrvInit();
	UsbDrvInit();
}
void appInit(void)
{
	usbAppInit();
}

int main(void)
{	
  driverInit();
  appInit();
	while (1)
	{
		UsbTask();
	}
}
