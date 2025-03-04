#include "MQTTClient.h"

void MQTTClient::MQTTConnect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    u32 len;
    u8 buf[200];

    data.clientID.cstring = this->getDnsName();
    data.keepAliveInterval = 2000;
    data.cleansession = 1;

    if((username != nullptr) && (password != nullptr))
    {
        data.username.cstring = username;
        data.password.cstring = password;
    }

    len = MQTTSerialize_connect(buf, sizeof(buf), &data);
    this->sendPacket(buf, len);
}

void MQTTClient::MQTTSubscribe( char *topic, int req_qos)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, msgid, 1, &topicString, &req_qos);
    this->sendPacket(buf, len);
}

void MQTTClient::MQTTUnsubscribe(char *topic)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_unsubscribe(buf, sizeof(buf), 0, msgid, 1, &topicString);
    this->sendPacket(buf, len);
}

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

void MQTTClient::MQTTPingreq(void)
{
    u32 len;
    u8 buf[10];

    len = MQTTSerialize_pingreq(buf,sizeof(buf));
    this->sendPacket(buf, len);
}

void MQTTClient::MQTTDisconnect(void)
{
    u32 len;
    u8 buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    this->sendPacket(buf, len);
}

bool MQTTClient::isMQTTConnected(void)
{
    return  (this->MQTTStatus == eMQTTStatus::MQTTConnected) || 
            (this->MQTTStatus == eMQTTStatus::MQTTSuback)    ||
            (this->MQTTStatus == eMQTTStatus::MQTTPublished);
        
}

eMQTTStatus MQTTClient::getMQTTStatus(void)
{
    return this->MQTTStatus;
}

void MQTTClient::parsePublishedTopic(uint8_t* buf, uint16_t len)
{
    MQTTDeserialize_publish(
                                &this->lastTopicDup,
                                &this->lastTopicQos,
                                &this->lastTopicRetained,
                                &this->lastTopicPacketID,
                                &this->lastTopicName,
                                &this->lastTopicPayload,
                                &this->lastTopicPayloadlen,
                                buf,
                                len
                            );
}

void MQTTClient::mainTask(void)
{
    uint16_t len = 0;
    uint8_t* recvBuf = this->getRecvBuf(&len);
    
    if(len > 0)
    {
        switch (recvBuf[0] >> 4)
        {
            case CONNACK:
                this->MQTTStatus = eMQTTStatus::MQTTConnected;
            break;

            case PUBLISH:
                this->MQTTStatus = eMQTTStatus::MQTTPublished;
                this->parsePublishedTopic(recvBuf, len);
                if(this->topicCallback != nullptr)
                {
                    this->topicCallback(
                                            this->lastTopicName.cstring, 
                                            this->lastTopicPayload, 
                                            this->lastTopicPayloadlen, 
                                            this->lastTopicQos, 
                                            this->lastTopicRetained, 
                                            this->lastTopicDup
                                        );
                }
                this->MQTTStatus = eMQTTStatus::MQTTConnected;
            break;

            case SUBACK:
                this->MQTTStatus = eMQTTStatus::MQTTSuback;
            break;
        
            default:
                this->MQTTStatus = eMQTTStatus::MQTTUnknown;
                this->unknownTmr = millis32();
            break;
        }

        this->flushRecvBuf();
    }

    switch (this->getSocketStatus())
    {
        case e_socketStatus::created:
            this->MQTTStatus = eMQTTStatus::MQTTUnknown;
        break;

        case e_socketStatus::disconnected:
            this->connect();
            this->MQTTStatus = eMQTTStatus::MQTTUnknown;
        break;

        case e_socketStatus::timeout:
            this->connect();
            this->MQTTStatus = eMQTTStatus::MQTTUnknown;
        break;

        case e_socketStatus::connected:
            if(!this->isMQTTConnected()) 
            {
                this->MQTTConnect();
                this->MQTTStatus = eMQTTStatus::MQTTConnectRequested;
            }
        break;

        case e_socketStatus::wrongstatus:
            this->connect();
            this->MQTTStatus = eMQTTStatus::MQTTUnknown;
        break;
    
        case e_socketStatus::receivied:
            this->MQTTStatus = eMQTTStatus::MQTTConnected;
        break;
    }

    if( (this->MQTTStatus == eMQTTStatus::MQTTUnknown) && ((millis32() - this->unknownTmr) > MQTT_UNKNOWN_MS_TIMEOUT) )
    {
        this->unknownTmr = millis32();
        this->MQTTConnect();
    }
}

void MQTTClient::registerTopicCallback(f_topicCallback cb)
{
    this->topicCallback = cb;
}