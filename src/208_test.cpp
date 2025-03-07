#include <ch32fun.h>
#include <ch32v20xhw.h>

#include "funny_defs.h"
#include "funny_time.h"
#include "simpleTimer.h"
#include "SystemInit120_HSE32.h"

//#include "ethIF.h"
//#include "tcpServer.h"
//#include "tcpClient.h"
#include "MQTTClient.h"

#include <cstdlib>

#include "GTimer.h"

//static uint8_t IPAddr[4]    = {0,0,0,0};//{ 192, 168, 1, 43 };                   //IP address
static uint8_t destIPAddr[4]    = { 192, 168, 1, 253 };                   //IP address
//static uint8_t GWIPAddr[4]  = {0,0,0,0};//{ 192, 168, 1, 1 };                    //Gateway IP address
//static uint8_t IPMask[4]    = {0,0,0,0};//{ 255, 255, 255, 0 };                  //subnet mask
//uint16_t srcport = 1000; 
#define SUB_TOPIC_COUNT 2

int main()
{  
    SystemInit120_HSE32();
	
    system_initSystick();

    //funGpioInitAll();

   ////ethIF myIF(IPAddr, GWIPAddr, IPMask);
   ethIF myIF;
   myIF.configKeepAlive();
   if(!myIF.init())
   {
        while (1){}  
   }

    MQTTClient<1UL> myClient(&myIF, destIPAddr);

    GTimer<millis32> myTimer(3000);
    myTimer.setMode(GTMode::Interval);
    myTimer.keepPhase(true);
    myTimer.start();

    char uptimeStr[128] = "ch32v208 uptime is  ";

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        myIF.mainTask();
        myClient.mainTask();

        if(myTimer)
        {
            char buf[14];

            itoa(millis32() / 1000, buf, 10);

            memcpy(uptimeStr + 19, buf, strlen(buf) + 1);
            strcat(uptimeStr, " seconds.");

            
            myClient.MQTTPublish((char*)"ch32topic/test/str", 0, uptimeStr);

        }

    }
}

