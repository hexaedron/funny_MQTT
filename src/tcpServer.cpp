#include "tcpServer.h"

tcpServer::tcpServer(ethIF* eth, uint16_t ipport)
{
    this->ethInterface = eth;
    this->srcport   = ipport;
    this->ethInterface->setSrvRetBuf(&this->retBuf);
    this->ethInterface->createTcpSocketListen(ipport);
}

tcpServer::~tcpServer()
{
}

void tcpServer::setIPPort(uint16_t port)
{
    this->srcport = port;
}

//bool tcpServer::init(void)
//{
//    this->tim2Init();          
//    
//    if (ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr) != WCHNET_ERR_SUCCESS) //Ethernet library initialize
//    {
//        return false;
//    }
//
//    if(this->keepAlive)
//    {
//        WCHNET_ConfigKeepLive(&cfg);
//    }
//    
//    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
//    
//    return this->createTcpSocketListen();
//}


/*********************************************************************
 * @fn      WCHNET_CreateTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */
//bool tcpServer::createTcpSocketListen(void)
//{
//    SOCK_INF TmpSocketInf;
//
//    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
//    TmpSocketInf.SourPort = srcport;
//    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
//    if(WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf) != WCHNET_ERR_SUCCESS)
//        return false;
//    if(WCHNET_SocketListen(SocketIdForListen) != WCHNET_ERR_SUCCESS)                   //listen for connections
//        return false;
//
//    return true;
//}
//

/*********************************************************************
 * @fn      sendPacket
 *
 * @brief   send data.
 *
 * @param   buf - data buff.
 *          len - data length
 *
 * @return  none
 */
void tcpServer::sendPacket(u8 *buf, u32 len)
{
    if(len > 0)
        this->ethInterface->sendSrvPacket(buf, len);
}

uint8_t* tcpServer::getRecvBuf(uint16_t* len)
{
    *len = this->retBuf.bufLen;
    return this->retBuf.retBuf;
}

void tcpServer::flushRecvBuf(void)
{
    this->retBuf.bufLen = 0;
}