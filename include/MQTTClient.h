#pragma once

#include "funny_time.h"

#include "tcpClient.h"
#include "MQTTPacket.h"

#define MQTT_UNKNOWN_MS_TIMEOUT 5000

enum eMQTTTopicTypes
{
    PUB, SUB
};

enum eMQTTStatus
{
    MQTTConnected,
    MQTTConnectRequested,
    MQTTPublished,
    MQTTSuback,
    MQTTUnknown
};

typedef void (*f_topicCallback)(char* topicName, uint8_t* topicPayload, int payloadLen, int topicQos, unsigned char retained, unsigned char dup ); 

class MQTTClient : public tcpClient
{
private:
    uint16_t MQTTPackedID = 5;
    eMQTTStatus MQTTStatus = eMQTTStatus::MQTTUnknown;
    MQTTString lastTopicName;
    uint8_t* lastTopicPayload;
    int lastTopicQos, lastTopicPayloadlen;
    unsigned char lastTopicRetained, lastTopicDup;
    unsigned short lastTopicPacketID;
    f_topicCallback topicCallback = nullptr;
    uint32_t unknownTmr;
    char* MQTTUsername = nullptr;
    char* MQTTPassword = nullptr;

    void parsePublishedTopic(uint8_t* buf, uint16_t len);
    void MQTTConnect(char *username = nullptr, char *password = nullptr);
    void sendMQTTPacket(u8 *buf, u32 len);
    bool isMQTTConnecRequested(void);

public:
    MQTTClient
                (
                    ethIF* eth, 
                    uint8_t* newDestIPAddress, 
                    uint16_t newDestIPPort = 1883, 
                    char* username = nullptr, 
                    char* password = nullptr
                ): tcpClient(eth, newDestIPAddress, newDestIPPort) 
    {
        this->MQTTUsername = username;
        this->MQTTPassword = password;
        this->unknownTmr = millis32();
    };

    void MQTTSubscribe( char *topic, int req_qos);
    void MQTTUnsubscribe(char *topic);
    void MQTTPublish(char *topic, int qos, char *payload);
    void MQTTPingreq(void);
    void MQTTDisconnect(void);
    bool isMQTTConnected(void);
    eMQTTStatus getMQTTStatus(void);
    void registerTopicCallback(f_topicCallback cb);
    void mainTask(void);
};


