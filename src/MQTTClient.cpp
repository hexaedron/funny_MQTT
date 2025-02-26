#include "MQTTClient.h"

//template<uint8_t pubTopicCount, uint8_t subTopicCount>
//bool MQTTClient<pubTopicCount, subTopicCount>::pushTopic(eMQTTTopicTypes type, const char* topic)
//{
//    if(type == eMQTTTopicTypes::PUB)
//    {
//        if(this->currentPubTopic >= pubTopicCount)
//        {
//            return false;
//        }
//        this->pubTopics[this->currentPubTopic] = topic;
//        this->currentPubTopic++;
//    }
//    else
//    {
//        if(this->currentSubTopic >= subTopicCount)
//        {
//            return false;
//        }
//        this->subTopics[this->currentSubTopic] = topic;
//        this->currentSubTopic++;
//    }
//
//    return true;
//}

//template<uint8_t pubTopicCount, uint8_t subTopicCount>
//void MQTTClient<pubTopicCount, subTopicCount>::setQOS(int newPubQOS, int newSubQOS)
//{
//    this->pub_qos = newPubQOS;
//    this->sub_qos = newSubQOS;
//}

//template<uint8_t pubTopicCount, uint8_t subTopicCount>
//void MQTTClient<pubTopicCount, subTopicCount>::MQTTConnect(char *username, char *password)
void MQTTClient::MQTTConnect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    u32 len;
    u8 buf[200];

    data.clientID.cstring = this->getDnsName();
    data.keepAliveInterval = 2000;
    data.cleansession = 1;
    data.username.cstring = username;
    data.password.cstring = password;

    len = MQTTSerialize_connect(buf, sizeof(buf), &data);
    this->sendPacket(buf, len);
}
/*
template<uint8_t pubTopicCount, uint8_t subTopicCount>
void MQTTClient<pubTopicCount, subTopicCount>::MQTTSubscribe( char *topic, int req_qos)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, msgid, 1, &topicString, &req_qos);
    this->sendPacket(buf, len);
}

template<uint8_t pubTopicCount, uint8_t subTopicCount>
void MQTTClient<pubTopicCount, subTopicCount>::MQTTUnsubscribe(char *topic)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_unsubscribe(buf, sizeof(buf), 0, msgid, 1, &topicString);
    this->sendPacket(buf, len);
}
*/

//template<uint8_t pubTopicCount, uint8_t subTopicCount>
//void MQTTClient<pubTopicCount, subTopicCount>::MQTTPublish(char *topic, int qos, char *payload)
void MQTTClient::MQTTPublish(char *topic, int qos, char *payload)
{
    MQTTString topicString = MQTTString_initializer;
    u32 payloadlen;
    u32 len;
    u8 buf[1024];

    topicString.cstring = topic;
    payloadlen = strlen(payload);
    len = MQTTSerialize_publish(buf, sizeof(buf), 0, qos, 0, this->MQTTPackedID++, topicString, (unsigned char*)payload, payloadlen);
    this->sendPacket(buf, len);
}
/*
template<uint8_t pubTopicCount, uint8_t subTopicCount>
void MQTTClient<pubTopicCount, subTopicCount>::MQTTPingreq(void)
{
    u32 len;
    u8 buf[10];

    len = MQTTSerialize_pingreq(buf,sizeof(buf));
    this->sendPacket(buf, len);
}

template<uint8_t pubTopicCount, uint8_t subTopicCount>
void MQTTClient<pubTopicCount, subTopicCount>::MQTTDisconnect(void)
{
    u32 len;
    u8 buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    this->sendPacket(buf, len);
}
    */