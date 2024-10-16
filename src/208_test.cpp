#include <ch32v003fun.h>
#include "funny_defs.h"

#include "funny_time.h"


#include <stdbool.h>
#include <cstdlib>
#include <cstring>

int main()
{
	SystemInit();
#ifdef WCH_FAST_INTERRUPT_ENABLED
	__set_INTSYSCR(0x3); // [Experimental] enable fast interrupt feature
#endif
	system_initSystick();

	
	funGpioInitAll();
	funPinMode(PC0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PC1, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

	while (true)
	{
		funDigitalWrite(PC0, FUN_LOW);
		funDigitalWrite(PC1, FUN_HIGH);
		Delay_Ms(200 );
		funDigitalWrite(PC0, FUN_HIGH);
		funDigitalWrite(PC1, FUN_LOW);
		Delay_Ms(200);
	}
}

