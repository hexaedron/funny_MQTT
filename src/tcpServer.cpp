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