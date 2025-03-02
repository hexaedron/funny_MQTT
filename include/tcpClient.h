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
    uint8_t socket = UINT8_MAX;

public:
    void setDestPort(uint16_t newDestIPPort);
    void setDestIP(uint8_t* newDestIPAddress);
    void sendPacket(u8 *buf, u32 len);
    char* getDnsName(void);
    uint8_t* getRecvBuf(uint16_t* len);  // Get the data we received from our socket
    void flushRecvBuf(void);
    bool disconnect(void);
    bool connect(void);
    e_socketStatus getSocketStatus(void);
    tcpClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort);
    ~tcpClient();
};