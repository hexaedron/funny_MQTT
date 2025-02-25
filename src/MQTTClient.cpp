#include "MQTTClient.h"

template<uint8_t pubTopicCount, uint8_t subTopicCount>
bool MQTTClient<pubTopicCount, subTopicCount>::pushTopic(eMQTTTopicTypes type, const char* topic)
{
    if(type == eMQTTTopicTypes::PUB)
    {
        if(this->currentPubTopic >= pubTopicCount)
        {
            return false;
        }
        this->pubTopics[this->currentPubTopic] = topic;
        this->currentPubTopic++;
    }
    else
    {
        if(this->currentSubTopic >= subTopicCount)
        {
            return false;
        }
        this->subTopics[this->currentSubTopic] = topic;
        this->currentSubTopic++;
    }

    return true;
}

template<uint8_t pubTopicCount, uint8_t subTopicCount>
void MQTTClient<pubTopicCount, subTopicCount>::setQOS(int newPubQOS, int newSubQOS)
{
    this->pub_qos = newPubQOS;
    this->sub_qos = newSubQOS;
}