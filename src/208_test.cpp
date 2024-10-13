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

	// We use button on PC6, so we need to init it and turn on interrupt.
	funGpioInitAll();
	funPinMode(PC6, GPIO_Speed_In | GPIO_CNF_IN_FLOATING);

	while (true)
	{
	}
}

