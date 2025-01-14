#include <ch32v003fun.h>
#include "funny_defs.h"

#include <cstdlib>

#include "funny_time.h"
#include "simpleTimer.h"

#include "SystemInit120_HSE32.h"


#include "eth_driver.h"

#include "tcpServer.h"

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

   tcpServer myServer(IPAddr, GWIPAddr, IPMask, srcport);
   myServer.configKeepAlive();
   if(!myServer.init())
   {
        while (1){}  
   }


    simpleTimer32 myTimer(2000UL);

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        myServer.mainTask();

        if(myTimer.ready())
        {
            //char buf[14];
            //itoa(millis32(), buf, 10);
            //size_t len = strlen(buf);
            //buf[len] = '\n';
            //buf[len + 1] = '\r';
            //myServer.sendPacket((uint8_t*) buf, len + 2);

            uint16_t length = 0;
            uint8_t* buf = myServer.getRecvBuf(&length);
            myServer.sendPacket(buf, length);
            myServer.flushRecvBuf();
        }

        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(myServer.queryGlobalInt())
        {
            myServer.handleGlobalInt();
        }
    }
}

