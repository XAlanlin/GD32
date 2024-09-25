#include <stdint.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "led_drv.h"
#include "key_drv.h"
#include "systick.h"
#include "usb_drv.h"

int main(void)
{	
	SystickInit();
	LedDrvInit();
	KeyDrvInit();
	UsbDrvInit();
	
	while (1)
	{
		UsbTask();
	}
}
