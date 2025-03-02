#pragma once

#include "tcpClient.h"
#include "MQTTPacket.h"


enum eMQTTTopicTypes
{
    PUB, SUB
};


class MQTTClient : public tcpClient
{
private:
    uint8_t currentPubTopic = 0;
    uint8_t currentSubTopic = 0;
    uint16_t MQTTPackedID = 5;
public:
    MQTTClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort = 1883): tcpClient(eth, newDestIPAddress, newDestIPPort) {};
    void MQTTConnect(char *username = nullptr, char *password = nullptr);
    void MQTTSubscribe( char *topic, int req_qos);
    void MQTTUnsubscribe(char *topic);
    void MQTTPublish(char *topic, int qos, char *payload);
    void MQTTPingreq(void);
    void MQTTDisconnect(void);
};


