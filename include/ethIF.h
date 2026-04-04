#pragma once
#include <ch32fun.h>
#include <ch32v20xhw.h>
#include <cstring>
#include "eth_driver.h"

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

class ethIF
{
private:
    uint8_t MACAddr[6];                 //MAC address
    uint8_t* IPAddr;                    //IP address
    uint8_t* GWIPAddr;                  //Gateway IP address
    uint8_t* IPMask;                    //subnet mask
    char dnsName[25] = "smartcube-";    //The DNS name we set by DHCP

    e_socketStatus   socketStates[WCHNET_MAX_SOCKET_NUM] = {e_socketStatus::created};
    uint8_t          SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
    int16_t          retBufLen[WCHNET_MAX_SOCKET_NUM] = {0};                 //Received data length for each socket
    bool             keepAlive = false;
    struct _KEEP_CFG cfg;
    e_phyStatus      phyStatus = e_phyStatus::unknown;

    void    dataLoopback(u8 id);
    void    tim2Init(void);
    void    mStopIfError(u8 iError);
    void    handleSockInt(u8 socketid, u8 intstat);
    void    handleGlobalInt(void);
    void    populateDNSName(void);
    
public:
    void            setIPAddr(uint8_t* addr);
    void            setGWIPAddr(uint8_t* addr);
    void            setIPMask(uint8_t* mask);
    bool            createTcpSocketListen(uint8_t* socketid, uint16_t port);
    bool            createTcpSocket(uint8_t* socketid, uint8_t* destIP, uint16_t destport, uint16_t srcport);
    bool            closeSocket(uint8_t socket);
    void            configKeepAlive(uint32_t KLIdle = 20000, uint32_t KLIntvl = 15000, uint32_t KLCount = 9);
    bool            init(void);
    void            mainTask(void); // Should be called in main cycle
    void            sendPacket(uint8_t socket, u8 *buf, u32 len); // Send a packet to a specific socket
    bool            isDHCPOK(void);
    char*           getDnsName(void);
    uint8_t*        getIPAddr(void) { return this->IPAddr; }
    e_phyStatus     getPHYStatus(void);
    e_socketStatus  getSocketStatus(uint8_t socketid);
    void            socketBufIsRead(uint8_t socketid);
    uint8_t*        getRecvBuf(uint8_t socketid, uint16_t* len);
    void            flushRecvBuf(uint8_t socketid);
    bool            isPHYOK(void);
    ethIF(uint8_t* IPAddr, uint8_t* GWIPAddr, uint8_t* IPMask); // Staic IP mode
    ethIF(); // DCHP mode
    ~ethIF();
};