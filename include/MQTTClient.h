#pragma once

#include <type_traits>

#include "funny_time.h"

#include "tcpClient.h"
#include "MQTTPacket.h"

enum eMQTTStatus
{
    MQTTConnected,
    MQTTConnectRequested,
    MQTTPublished,
    MQTTSuback,
    MQTTPuback,
    MQTTTPubrec,
    MQTTTPubrel,
    MQTTTPubcomp,
    MQTTUnsuback,
    MQTTUnknown
};

// Stubs (C++20)
struct emptyMQTTArray {};
struct emptyQoSArray  {};
struct emptyCharArray {};


typedef void (*f_topicCallback)(char* topicName, uint8_t* topicPayload, int payloadLen, int topicQos, unsigned char retained, unsigned char dup ); 

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength       = 70, 
    size_t subTopicPayloadLength    = 128,
    size_t willTopicNameLength      = 70,
    size_t willTopicPayloadLength   = 20
>
class MQTTClient : public tcpClient
{
private:
    uint16_t        MQTTPackedID             = 5;
    eMQTTStatus     MQTTStatus               = eMQTTStatus::MQTTUnknown;
    MQTTString      lastTopicName;
    uint8_t*        lastTopicPayload;
    int             lastTopicQos, lastTopicPayloadlen;
    unsigned char   lastTopicRetained, lastTopicDup;
    unsigned short  lastTopicPacketID;
    f_topicCallback topicCallback            = nullptr;
    uint32_t        unknownTmr, pingreqTmr, pingreqPrd;
    char*           MQTTUsername             = nullptr;
    char*           MQTTPassword             = nullptr;

    // We choose a type here: array or stub
    using topicsStorage = std::conditional_t
    <
        (subTopicCount > 0),
        MQTTString[subTopicCount],  // true → array
        emptyMQTTArray              // false → stub
    >;
    [[no_unique_address]] topicsStorage subTopics;

    // The same for QoS
    using QoSStorage = std::conditional_t
    <
        (subTopicCount > 0),
        int[subTopicCount],  // true → array
        emptyQoSArray        // false → array
    >;
    [[no_unique_address]] QoSStorage subQoSs;

    // The same for topic strings
    using CharStorage = std::conditional_t
    <
        (subTopicCount > 0),
        char[subTopicCount][subTopicNameLength],  // true → array
        emptyCharArray                            // false → array
    >;
    [[no_unique_address]] CharStorage topicStorage;

    unsigned short  keepAlive;
    char            willTopic[willTopicNameLength]{0};
    char            willMessage[willTopicPayloadLength]{0};
    int             willQoS{1};

    void parsePublishedTopic(uint8_t* buf, uint16_t len);
    void MQTTConnect(char *username = nullptr, char *password = nullptr);
    void sendMQTTPacket(u8 *buf, u32 len);
    bool isMQTTConnectRequested(void);
    void MQTTSubscribe(void);

public:
    MQTTClient
                (
                    ethIF* eth, 
                    uint8_t* newDestIPAddress, 
                    uint16_t newDestIPPort = 1883,
                    unsigned int keepalive = 60,
                    char* username = nullptr, 
                    char* password = nullptr,
                    uint32_t setpingreqPrd = 3000 // Send pingreq every 3 seconds by default
                ): tcpClient(eth, newDestIPAddress, newDestIPPort) 
    {
        this->MQTTUsername = username;
        this->MQTTPassword = password;
        this->unknownTmr   = millis32();
        this->pingreqTmr   = this->unknownTmr;
        this->pingreqPrd   = setpingreqPrd;
        this->keepAlive    = keepalive;
        if constexpr (subTopicCount > 0)
        {
            for(uint32_t i = 0; i < subTopicCount; i++)
            {
                this->subTopics[i] = MQTTString_initializer;
                this->subQoSs[i] = 0;
            }
        }
    };

    void        MQTTUnsubscribe(char *topic);
    void        MQTTPublish(char *topic, int qos, char *payload, bool retained = false);
    void        MQTTPingreq(void);
    void        MQTTDisconnect(void);
    bool        isMQTTConnected(void);
    eMQTTStatus getMQTTStatus(void);
    void        registerTopicCallback(f_topicCallback cb);
    void        mainTask(uint32_t unknownTimeout = 5000); // Should be called in the main cycle
    void        addSubTopic(char* name, int qos = 0);
    void        addWillTopic(char* name, char* message, int qos = 1);
};

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::addWillTopic(char* name, char* message, int qos)
{
    strcpy(this->willMessage, message);
    this->willQoS = qos;
    strcpy(this->willTopic, name);
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTConnect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    u32 len;
    u8 buf[200];

    data.clientID.cstring = this->getDnsName();
    data.keepAliveInterval = this->keepAlive;
    data.cleansession = 1;

    if((this->willMessage[0] != 0) && (this->willTopic[0] != 0))
    {
        data.willFlag = 1;
        data.will.message.cstring = this->willMessage;
        data.will.topicName.cstring = this->willTopic;
        data.will.qos = this->willQoS;
    } 

    if((username != nullptr) && (password != nullptr))
    {
        data.username.cstring = username;
        data.password.cstring = password;
    }

    len = MQTTSerialize_connect(buf, sizeof(buf), &data);
    this->sendPacket(buf, len);
}

// Use this function to add subscritions one by one.
// Don't add more than subTopicCount (i.e. template parameter) functions
template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::addSubTopic(char* topicName, int qos)
{
    static uint32_t i = 0;

    if constexpr (subTopicCount > 0)
    {
        if(i < subTopicCount)
        {
            strcpy(this->topicStorage[i], topicName);
            this->subTopics[i].cstring = topicStorage[i];
            this->subQoSs[i] = qos;
        }

        i++;
    }
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTSubscribe(void)
{
    u32 len;
    u32 msgid = 1;
    u8 buf[1024];

    if constexpr (subTopicCount > 0)
    {
        len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, msgid, subTopicCount, this->subTopics, this->subQoSs);
        this->sendPacket(buf, len);
    }
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTUnsubscribe(char *topic)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_unsubscribe(buf, sizeof(buf), 0, msgid, 1, &topicString);
    this->sendPacket(buf, len);
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTPublish(char *topic, int qos, char *payload, bool retained)
{
    MQTTString topicString = MQTTString_initializer;
    u32 payloadlen;
    u32 len;
    u8 buf[1024];

    topicString.cstring = topic;
    payloadlen = strlen(payload);
    len = MQTTSerialize_publish(buf, sizeof(buf), 0, qos, retained, this->MQTTPackedID++, topicString, (unsigned char*)payload, payloadlen);
    this->sendPacket(buf, len);
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTPingreq(void)
{
    u32 len;
    u8 buf[10];

    len = MQTTSerialize_pingreq(buf, sizeof(buf));
    this->sendPacket(buf, len);
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::MQTTDisconnect(void)
{
    u32 len;
    u8 buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    this->sendPacket(buf, len);
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
bool MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::isMQTTConnected(void)
{
    return  this->MQTTStatus != eMQTTStatus::MQTTUnknown;      
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
bool MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::isMQTTConnectRequested(void)
{
    return  (this->MQTTStatus == eMQTTStatus::MQTTConnectRequested);
        
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
eMQTTStatus MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::getMQTTStatus(void)
{
    return this->MQTTStatus;
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::parsePublishedTopic(uint8_t* buf, uint16_t len)
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

// MQTT client main task function, should be called cyclically
template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::mainTask(uint32_t unknownTimeout)
{
    uint16_t len = 0;
    uint8_t* recvBuf = this->getRecvBuf(&len);
    
    // Parse received data to determine further actions and set a correct client status
    if(len > 0)
    {
        switch (recvBuf[0] >> 4)
        {
            case CONNACK:
                this->MQTTStatus = eMQTTStatus::MQTTConnected;
                this->MQTTSubscribe();
            break;

            case PINGRESP:
                this->MQTTStatus = eMQTTStatus::MQTTConnected;
            break;

            case PUBLISH:
                this->MQTTStatus = eMQTTStatus::MQTTPublished;
                this->parsePublishedTopic(recvBuf, len);

                if(this->lastTopicQos == 1)
                {
                    // Send PUBACK
                    u32 plen;
                    u8 pbuf[20];
                    plen = MQTTSerialize_puback(pbuf, sizeof(pbuf), this->lastTopicPacketID);
                    this->sendPacket(pbuf, plen);
                }
                else if(this->lastTopicQos == 2)
                {
                    // Send PUBREC
                    u32 plen;
                    u8 pbuf[20];
                    plen = MQTTSerialize_pubrec(pbuf, sizeof(pbuf), this->lastTopicPacketID);
                    this->sendPacket(pbuf, plen);
                }

                if(this->topicCallback != nullptr)
                {
                    char buf[subTopicNameLength]{'\0'};
                    memcpy(buf, this->lastTopicName.lenstring.data, this->lastTopicName.lenstring.len);
                    this->topicCallback(
                                            buf, 
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

            case PUBREC:
                this->MQTTStatus = eMQTTStatus::MQTTTPubrec;
            break;

            case PUBREL:
                this->MQTTStatus = eMQTTStatus::MQTTTPubrel;
                u32 plen;
                u8 pbuf[20];
                unsigned char packetType, paketDup;
                unsigned short packetID;
                plen = MQTTDeserialize_ack(&packetType, &paketDup, &packetID, recvBuf, len);
                plen = MQTTSerialize_pubcomp(pbuf, sizeof(pbuf), this->lastTopicPacketID);
                this->sendPacket(pbuf, plen);
            break;

            case PUBCOMP:
                this->MQTTStatus = eMQTTStatus::MQTTTPubcomp;
            break;

            case PUBACK:
                this->MQTTStatus = eMQTTStatus::MQTTPuback;
            break;

            case UNSUBACK:
                this->MQTTStatus = eMQTTStatus::MQTTUnsuback;
            break;

            case DISCONNECT:
            default:
                this->MQTTStatus = eMQTTStatus::MQTTUnknown;
                this->unknownTmr = millis32();
            break;
        }

        this->flushRecvBuf();
    }

    // Check socket status to determine further actions and set a correct client status
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
            if( !(this->isMQTTConnected() || this->isMQTTConnectRequested()) ) 
            {
                this->MQTTConnect(this->MQTTUsername, this->MQTTPassword);
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

    uint32_t time = millis32();

    // fix millis32 rollup
    if(time < this->unknownTmr )
    {
        this->unknownTmr = time;
    }
    // Check that it client is no longer in MQTTUnknown for more than unknownTmr
    if( (this->MQTTStatus == eMQTTStatus::MQTTUnknown) && ((time - this->unknownTmr) > unknownTimeout) )
    {
        this->unknownTmr = time;
        this->MQTTConnect(this->MQTTUsername, this->MQTTPassword);
        this->MQTTStatus = eMQTTStatus::MQTTConnectRequested;
    }

    
    // fix millis32 rollup
    if(time < this->pingreqTmr )
    {
        this->pingreqTmr = time;
    }
    // Send PINGREQ every pingreqPrd seconds
    if( (time - this->pingreqTmr) > this->pingreqPrd )
    {
        this->pingreqTmr = time;
        this->MQTTPingreq();
    }
}

// We set a callback funtion here. 
// This function is being called each time the MQTT client receives any topic.
// So all the incoming topic handling should be done inside that callback.
template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::registerTopicCallback(f_topicCallback cb)
{
    this->topicCallback = cb;
}

template 
<
    uint32_t subTopicCount, 
    size_t subTopicNameLength, 
    size_t subTopicPayloadLength,
    size_t willTopicNameLength,
    size_t willTopicPayloadLength
>
void MQTTClient
<
    subTopicCount, 
    subTopicNameLength, 
    subTopicPayloadLength, 
    willTopicNameLength, 
    willTopicPayloadLength
>::sendMQTTPacket(u8 *buf, u32 len)
{
    if(this->isMQTTConnected())
    {
        this->sendPacket(buf, len);
    }
}
