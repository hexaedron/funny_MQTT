#include "tcpServer.h"

tcpServer::tcpServer(ethIF* eth, uint16_t ipport)
{
    this->ethInterface = eth;
    this->dstport   = ipport;
    this->ethInterface->createTcpSocketListen(&this->socketForListen, ipport);
}

tcpServer::~tcpServer()
{
}

void tcpServer::setIPPort(uint16_t port)
{
    this->dstport = port;
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
    {
        this->ethInterface->sendPacket(this->socket, buf, len);
    }
}

uint8_t* tcpServer::getRecvBuf(uint16_t* len)
{
    if(this->socket == UINT8_MAX)
    {
        for(uint32_t i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            if
            ( 
                (SocketInf[i].SourPort == this->dstport) && 
                (SocketInf[i].DesPort != 0)              && 
                (SocketInf[i].ProtoType == PROTO_TYPE_TCP) 
            )
            {
                this->socket = i;
                return this->ethInterface->getRecvBuf(this->socket, len);
            }
        }

        *len = 0;
        return nullptr; 
    }
    
    return this->ethInterface->getRecvBuf(this->socket, len);
}

void tcpServer::flushRecvBuf(void)
{
    this->ethInterface->flushRecvBuf(this->socket);
}