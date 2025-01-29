#include <ch32v003fun.h>

#include "funny_defs.h"
#include "funny_time.h"
#include "simpleTimer.h"
#include "SystemInit120_HSE32.h"

#include "eth_driver.h"
#include "ethIF.h"

#include <cstdlib>

#include "GTimer.h"

static uint8_t IPAddr[4]    = { 192, 168, 1, 43 };                   //IP address
static uint8_t GWIPAddr[4]  = { 192, 168, 1, 1 };                    //Gateway IP address
static uint8_t IPMask[4]    = { 255, 255, 255, 0 };                  //subnet mask
uint16_t srcport = 1000; 


int main()
{  
    SystemInit120_HSE32();


#ifdef WCH_FAST_INTERRUPT_ENABLED
	__set_INTSYSCR(0x3); // [Experimental] enable fast interrupt feature
#endif
	
    system_initSystick();

    //funGpioInitAll();

   ethIF myIF(IPAddr, GWIPAddr, IPMask);
   myIF.configKeepAlive();
   if(!myIF.init())
   {
        while (1){}  
   }


    //simpleTimer32 myTimer(1000UL);
    GTimer<millis32> myTimer(1000);
    myTimer.setMode(GTMode::Interval);
    myTimer.keepPhase(true);
    myTimer.start();

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        myIF.mainTask();

        if(myTimer)
        {
            //char buff[14];
            //itoa(millis32(), buff, 10);
            //size_t len = strlen(buff);
            //buff[len] = '\n';
            //buff[len + 1] = '\r';
            //myServer.sendPacket((uint8_t*) buff, len + 2);

            //uint16_t length = 0;
            //uint8_t* buf = myServer.getRecvBuf(&length);
            //myServer.sendPacket(buf, length);
            //myServer.flushRecvBuf();
        }

        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(myIF.queryGlobalInt())
        {
            myIF.handleGlobalInt();
        }
    }
}

