#include "MQTTClient.h"

void MQTTClient::MQTTConnect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    u32 len;
    u8 buf[200];

    data.clientID.lenstring.data = this->getDnsName();
    data.clientID.lenstring.len = strlen(this->getDnsName());
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