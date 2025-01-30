#pragma once
#include <ch32v003fun.h>
#include <cstring>
#include "eth_driver.h"

typedef struct
{
    uint8_t retBuf[RECE_BUF_LEN];
    uint16_t bufLen = 0;
} sRetBuf;


class ethIF
{
private:
    uint8_t MACAddr[6];                                          //MAC address
    uint8_t* IPAddr;                     //IP address
    uint8_t* GWIPAddr;                    //Gateway IP address
    uint8_t* IPMask;                    //subnet mask

    uint8_t SocketIdForListen = UINT8_MAX;                                   //Socket for Listening
    uint8_t socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
    uint8_t SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
    //uint8_t retBuf[RECE_BUF_LEN];
    //uint16_t bufLen = 0;
    sRetBuf* srvRetBuf;
    bool keepAlive = false;
    struct _KEEP_CFG cfg;

    void dataLoopback(u8 id);
    //bool createTcpSocketListen(void);
    void tim2Init(void);
    void mStopIfError(u8 iError);
    void handleSockInt(u8 socketid, u8 intstat);
    void handleGlobalInt(void);

public:
    void setSrvRetBuf(sRetBuf* newRetBuf);
    void setIPAddr(uint8_t* addr);
    void setGWIPAddr(uint8_t* addr);
    void setIPMask(uint8_t* mask);
    bool createTcpSocketListen(uint16_t port);
    //void setIPPort(uint16_t port);
    void configKeepAlive(uint32_t KLIdle = 20000, uint32_t KLIntvl = 15000, uint32_t KLCount = 9);
    bool init(void);
    void mainTask(void);
    void sendSrvPacket(u8 *buf, u32 len);
    //uint8_t* getRecvBuf(uint16_t* len);
    //void flushRecvBuf(void);
    ethIF(uint8_t* IPAddr, uint8_t* GWIPAddr, uint8_t* IPMask);
    ethIF(); // DCHP mode
    ~ethIF();
};