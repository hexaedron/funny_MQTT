#pragma once

#include "tcpClient.h"

enum eMQTTTopicTypes
{
    PUB, SUB
};


template <uint8_t pubTopicCount, uint8_t subTopicCount>
class MQTTClient : public tcpClient
{
private:
    char* pubTopics[pubTopicCount];
    char* subTopics[subTopicCount];
    uint8_t currentPubTopic = 0;
    uint8_t currentSubTopic = 0;
    int pub_qos = 0;                                   //Publish quality of service
    int sub_qos = 0;                                   //Subscription quality of service
public:
    MQTTClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort): tcpClient(eth, newDestIPAddress, newDestIPPort) {};
    bool pushTopic(eMQTTTopicTypes type, const char* topic);
    void setQOS(int newPubQOS, int newSubQOS);
};


