#include "tcpClient.h"

tcpClient::tcpClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort)
{
    this->ethInterface = eth;
    this->destIPAddress = newDestIPAddress;
    this->destIPPort = newDestIPPort;
    this->ethInterface->setSrvRetBuf(&this->retBuf);
    this->ethInterface->createTcpSocket(&this->socket, this->destIPAddress, this->destIPPort);
}

tcpClient::~tcpClient()
{
}

void tcpClient::setDestPort(uint16_t newDestIPPort)
{
    this->destIPPort = newDestIPPort;
}

void tcpClient::setDestIP(uint8_t* newDestIPAddress)
{
    this->destIPAddress = newDestIPAddress;
}

void tcpClient::sendPacket(u8 *buf, u32 len)
{
    if(len > 0)
    {
        this->ethInterface->sendPacket(this->socket, buf, len);
    }
}

uint8_t* tcpClient::getRecvBuf(uint16_t* len)
{
    *len = this->retBuf.bufLen;
    return this->retBuf.retBuf;
}

void tcpClient::flushRecvBuf(void)
{
    this->retBuf.bufLen = 0;
}

bool tcpClient::disconnect(void)
{
    return this->ethInterface->closeSocket(this->socket);
}

bool tcpClient::connect(void)
{
    return this->ethInterface->createTcpSocket(&this->socket, this->destIPAddress, this->destIPPort);
}

char* tcpClient::getDnsName(void)
{
    return this->ethInterface->getDnsName();
}