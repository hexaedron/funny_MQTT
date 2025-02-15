#pragma once
#include <ch32fun.h>
#include <ch32v20xhw.h>
#include <cstring>
#include "ethIF.h"

class tcpClient
{
private:
    ethIF* ethInterface;
    sRetBuf retBuf;
    uint8_t* destIPAddress;                     //IP address
    uint16_t destIPPort;

public:
    void setDestPort(uint16_t newDestIPPort);
    void setDestIP(uint8_t* newDestIPAddress);
    void sendPacket(u8 *buf, u32 len);
    uint8_t* getRecvBuf(uint16_t* len);
    void flushRecvBuf(void);
    tcpClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort);
    ~tcpClient();
};