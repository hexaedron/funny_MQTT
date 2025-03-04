#pragma once

#include "tcpClient.h"
#include "MQTTPacket.h"


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
    void parsePublishedTopic(uint8_t* buf, uint16_t len);
    MQTTString lastTopicName;
    uint8_t* lastTopicPayload;
    int lastTopicQos, lastTopicPayloadlen;
    unsigned char lastTopicRetained, lastTopicDup;
    unsigned short lastTopicPacketID;
    f_topicCallback topicCallback = nullptr;
public:
    MQTTClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort = 1883): tcpClient(eth, newDestIPAddress, newDestIPPort) {};
    void MQTTConnect(char *username = nullptr, char *password = nullptr);
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


