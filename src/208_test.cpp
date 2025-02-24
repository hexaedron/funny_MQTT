#include <ch32fun.h>
#include <ch32v20xhw.h>

#include "funny_defs.h"
#include "funny_time.h"
#include "simpleTimer.h"
#include "SystemInit120_HSE32.h"

#include "ethIF.h"
#include "tcpServer.h"
#include "tcpClient.h"

#include <cstdlib>

#include "GTimer.h"

//static uint8_t IPAddr[4]    = {0,0,0,0};//{ 192, 168, 1, 43 };                   //IP address
static uint8_t destIPAddr[4]    = { 192, 168, 1, 20 };                   //IP address
//static uint8_t GWIPAddr[4]  = {0,0,0,0};//{ 192, 168, 1, 1 };                    //Gateway IP address
//static uint8_t IPMask[4]    = {0,0,0,0};//{ 255, 255, 255, 0 };                  //subnet mask
//uint16_t srcport = 1000; 


int main()
{  
    SystemInit120_HSE32();
	
    system_initSystick();

    //funGpioInitAll();

   //ethIF myIF(IPAddr, GWIPAddr, IPMask);
   ethIF myIF;
   myIF.configKeepAlive();
   if(!myIF.init())
   {
        while (1){}  
   }

   //tcpServer myServer(&myIF, 1000);
   tcpClient myClient(&myIF, destIPAddr, 10000);

    GTimer<millis32> myTimer(3000);
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
            char buff[14];
            itoa(millis32(), buff, 10);
            size_t len = strlen(buff);
            buff[len] = '\n';
            buff[len + 1] = '\r';
            //myServer.sendPacket((uint8_t*) buff, len + 2);
            myClient.sendPacket((uint8_t*) buff, len + 2);

            uint16_t length = 0;
            uint8_t* buf = myClient.getRecvBuf(&length);
            myClient.sendPacket(buf, length);
            myClient.flushRecvBuf();

            //uint16_t length = 0;
            //uint8_t* buf = myServer.getRecvBuf(&length);
            //myServer.sendPacket(buf, length);
            //myServer.flushRecvBuf();
        }

    }
}

