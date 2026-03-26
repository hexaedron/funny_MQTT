#include "tcpClient.h"

tcpClient::tcpClient(ethIF* eth, uint8_t* newDestIPAddress, uint16_t newDestIPPort, uint16_t newSrcPort)
{
    this->ethInterface = eth;
    this->destIPAddress = newDestIPAddress;
    this->destIPPort = newDestIPPort;
    this->srcport = newSrcPort;
    this->ethInterface->createTcpSocket(&this->socket, this->destIPAddress, this->destIPPort, this->srcport++);
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
    return this->ethInterface->getRecvBuf(this->socket, len);
}

void tcpClient::flushRecvBuf(void)
{
    this->ethInterface->flushRecvBuf(this->socket);
    this->ethInterface->socketBufIsRead(this->socket);
}

bool tcpClient::disconnect(void)
{
    return this->ethInterface->closeSocket(this->socket);
}

bool tcpClient::connect(void)
{
    return this->ethInterface->createTcpSocket(&this->socket, this->destIPAddress, this->destIPPort, this->srcport);
}

char* tcpClient::getDnsName(void)
{
    return this->ethInterface->getDnsName();
}

e_socketStatus tcpClient::getSocketStatus(void)
{
    return this->ethInterface->getSocketStatus(this->socket);
}