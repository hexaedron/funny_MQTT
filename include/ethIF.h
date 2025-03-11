#pragma once
#include <ch32fun.h>
#include <ch32v20xhw.h>
#include <cstring>
#include "eth_driver.h"

typedef struct
{
    uint8_t retBuf[RECE_BUF_LEN];
    uint16_t bufLen = 0;
} sRetBuf;

typedef enum
{
    unreach,
    ipConflict,
    linkSuccess,
    unknown
} e_phyStatus;

typedef enum
{
    receivied,
    connected,
    disconnected,
    timeout,
    created,
    wrongstatus
} e_socketStatus;

typedef struct 
{
    uint8_t socketID;
    e_socketStatus status;
} s_socket;


class ethIF
{
private:
    uint8_t MACAddr[6];                 //MAC address
    uint8_t* IPAddr;                    //IP address
    uint8_t* GWIPAddr;                  //Gateway IP address
    uint8_t* IPMask;                    //subnet mask
    uint16_t srcport;
    char dnsName[25] = "SMARTCUBE-";    //The DNS name we set by DHCP

    s_socket         socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
    uint8_t          SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
    sRetBuf*         srvRetBuf;
    bool             keepAlive = false;
    struct _KEEP_CFG cfg;
    e_phyStatus      phyStatus = e_phyStatus::unknown;

    void    dataLoopback(u8 id);
    void    tim2Init(void);
    void    mStopIfError(u8 iError);
    void    handleSockInt(u8 socketid, u8 intstat);
    void    handleGlobalInt(void);
    void    populateDNSName(void);
    uint8_t getSocketNumByID(uint8_t sokcetid);

public:
    void            setSrvRetBuf(sRetBuf* newRetBuf);
    void            setIPAddr(uint8_t* addr);
    void            setGWIPAddr(uint8_t* addr);
    void            setIPMask(uint8_t* mask);
    bool            createTcpSocketListen(uint8_t* socketid, uint16_t port);
    bool            createTcpSocket(uint8_t* socketid, uint8_t* destIP, uint16_t destport);
    bool            closeSocket(uint8_t socket);
    void            configKeepAlive(uint32_t KLIdle = 20000, uint32_t KLIntvl = 15000, uint32_t KLCount = 9);
    bool            init(void);
    void            mainTask(void); // Should be called in main cycle
    void            sendPacket(uint8_t socket, u8 *buf, u32 len); // Send a packet to a specific socket
    bool            isDHCPOK(void);
    char*           getDnsName(void);
    e_phyStatus     getPHYStatus(void);
    e_socketStatus  getSocketStatus(uint8_t socketid);
    void            socketBufIsRead(uint8_t socketid);
    bool            isPHYOK(void);
    ethIF(uint8_t* IPAddr, uint8_t* GWIPAddr, uint8_t* IPMask, uint16_t newSrcPort = 55555); // Staic IP mode
    ethIF(uint16_t newSrcPort = 55555); // DCHP mode
    ~ethIF();
};