#pragma once
#include <ch32fun.h>
#include <ch32v20xhw.h>
#include <cstring>
#include "ethIF.h"

class tcpServer
{
private:
    uint16_t srcport;                                    //source port
    ethIF* ethInterface;
    sRetBuf retBuf;
    uint8_t socket = UINT8_MAX;

public:
    void setIPPort(uint16_t port);
    void sendPacket(u8 *buf, u32 len);
    uint8_t* getRecvBuf(uint16_t* len);
    void flushRecvBuf(void);
    tcpServer(ethIF* eth, uint16_t IPPort);
    ~tcpServer();
};