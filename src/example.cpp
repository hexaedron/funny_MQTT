#include <ch32fun.h>
#include <ch32v20xhw.h>

#include "funny_defs.h"
#include "funny_time.h"
#include "simpleTimer.h"
#include "SystemInit120_HSE32.h"


#include "MQTTClient.h"

#include <cstdlib>

#include "GTimer.h"

//static uint8_t IPAddr[4]    = {0,0,0,0};//{ 192, 168, 1, 43 };                   //IP address
static uint8_t destIPAddr[4]    = { 192, 168, 1, 253 };                   //IP address
//static uint8_t GWIPAddr[4]  = {0,0,0,0};//{ 192, 168, 1, 1 };                    //Gateway IP address
//static uint8_t IPMask[4]    = {0,0,0,0};//{ 255, 255, 255, 0 };                  //subnet mask
//uint16_t srcport = 1000; 
#define SUB_TOPIC_COUNT 2UL

char receivedBuf[128] = "Received message: ";
bool msgFlag = false;

// This function is being called each time the MQTT client receives any topic.
// So all the incoming topic handling should be done there.
void topicCallback(char* topicName, uint8_t* topicPayload, int payloadLen, int topicQos, unsigned char retained, unsigned char dup )
{
    memcpy(receivedBuf + 18, topicPayload, payloadLen);
    receivedBuf[payloadLen + 17] = '\0';
    msgFlag = true;
}

int main()
{  
    // Set up system clock to 120MHz, or else ethernet will refuse to work
    SystemInit120_HSE32();
	
    system_initSystick();

    // Create an ethIF object. It is used by MQTT client
    //ethIF myIF(IPAddr, GWIPAddr, IPMask);
    ethIF myIF; // DHCP mode
    myIF.configKeepAlive();
    if(!myIF.init())
    {
         while (1){}  
    }

    // Create MQTTClient object. It is VERY IMPORTANT to set the template parameter 
    // to be equal to exact number of topic filters we are going subscribe to
    MQTTClient<SUB_TOPIC_COUNT> myClient(&myIF, destIPAddr, 1883, 10);

    // Subscribe to topics
    myClient.addSubTopic((char*)"ch32topic/test/cmd1");
    myClient.addSubTopic((char*)"ch32topic/test_1/cmd2");

    // Register the above topic callback function.
    myClient.registerTopicCallback(topicCallback);

    // [OPTIONAL] Set the last will topic, its payload and QoS level
    myClient.addWillTopic((char*)"ch32topic/test/will", myIF.getDnsName());

    // Init software timer based on millis32()
    GTimer<millis32> myTimer(3000);
    myTimer.setMode(GTMode::Interval);
    myTimer.keepPhase(true);
    myTimer.start();

    // We will be publishing uptime message every 3 seconds
    char uptimeStr[128] = "ch32v208 uptime is  ";

    while(1)
    {
        myIF.mainTask();        // Ethernet library main task function, should be called cyclically
        myClient.mainTask();    // The same for MQTT client main task

        if(myTimer)
        {
            // Construct a message
            char buf[14];
            itoa(millis32() / 1000, buf, 10);
            memcpy(uptimeStr + 19, buf, strlen(buf) + 1);
            strcat(uptimeStr, " seconds.");
            
            // And publish it
            myClient.MQTTPublish((char*)"ch32topic/test/str", 0, uptimeStr);
        }

        if(msgFlag)
        {
            // Here we publish back all the messages we got from topics we subscribed above
            myClient.MQTTPublish((char*)"ch32topic/test/receivedstr", 0, receivedBuf);
            msgFlag = false;
        }

    }
}

