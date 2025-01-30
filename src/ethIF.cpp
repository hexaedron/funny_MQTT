#include "ethIF.h"
#include <cstdlib>

ethIF::ethIF(uint8_t* ipaddr, uint8_t* gwipaddr, uint8_t* ipmask)
{
    WCHNET_GetMacAddr(this->MACAddr);
    this->IPAddr    = ipaddr;
    this->GWIPAddr  = gwipaddr;
    this->IPMask    = ipmask;
}

ethIF::ethIF()
{
    #define PREFIX_LEN 10
    char buf[PREFIX_LEN + 13] = "SMARTCUBE-";
    const char symbols[17] = "0123456789ABCDEF";
    
    for (int i = 0; i < 6; i++) 
    {
        // Преобразуем каждый байт в два символа в 16-ричной системе
        buf[PREFIX_LEN + i*2] = symbols[MACAddr[i] >> 4];   // Старший nibble
        buf[PREFIX_LEN + i*2 + 1] = symbols[MACAddr[i] & 0x0F]; // Младший nibble
    }
    buf[PREFIX_LEN + 12] = '\0';

    WCHNET_DHCPSetHostname(buf);
    #undef PREFIX_LEN
}

ethIF::~ethIF()
{
}

void ethIF::setIPAddr(uint8_t* addr)
{
    this->IPAddr = addr;
}

void ethIF::setGWIPAddr(uint8_t* addr)
{
    this->GWIPAddr = addr;
}

void ethIF::setIPMask(uint8_t* mask)
{
    this->IPMask = mask;
}

//void ethIF::setIPPort(uint16_t port)
//{
//    this->srcport = port;
//}

void ethIF::configKeepAlive(uint32_t KLIdle, uint32_t KLIntvl, uint32_t KLCount)
{
    this->keepAlive   = true;
    this->cfg.KLIdle  = KLIdle;
    this->cfg.KLIntvl = KLIntvl;
    this->cfg.KLCount = KLCount;
}

bool ethIF::init(void)
{
    this->tim2Init();          
    
    if (ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr) != WCHNET_ERR_SUCCESS) //Ethernet library initialize
    {
        return false;
    }

    if(this->keepAlive)
    {
        WCHNET_ConfigKeepLive(&cfg);
    }
    
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    
    //return this->createTcpSocketListen();
    return true;
}


/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void ethIF::mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void ethIF::tim2Init(void)
{
    // Enable TIM2
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
    // Reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;

	TIM2->CTLR1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS)));
	TIM2->CTLR1 |= TIM_CounterMode_Up;

	TIM2->CTLR1 &= ~(TIM_CTLR1_CKD);

    TIM2->ATRLR = (120000000UL / 1000000UL);
    TIM2->PSC = (uint16_t)(WCHNETTIMERPERIOD * 1000 - 1);

	TIM2->DMAINTENR |= TIM_IT_Update;

	TIM2->CTLR1 |= TIM_CEN;

	TIM2->INTFR = ~TIM_IT_Update;
	
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */
bool ethIF::createTcpSocketListen(uint16_t port)
{
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = port;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    if(WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf) != WCHNET_ERR_SUCCESS)
        return false;
    if(WCHNET_SocketListen(SocketIdForListen) != WCHNET_ERR_SUCCESS)                   //listen for connections
        return false;

    return true;
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void ethIF::dataLoopback(u8 id)
{
    uint32_t len, totallen;
    uint8_t *p = this->srvRetBuf->retBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    //printf("Receive Len = %d\r\n", len);
    totallen = len;
    this->srvRetBuf->bufLen = len; 
    WCHNET_SocketRecv(id, this->srvRetBuf->retBuf, &len);                                  //Read the data of the receive buffer into retBuf
    while(1)
    {
        len = totallen;
        //WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void ethIF::handleSockInt(u8 socketid, u8 intstat)
{
    u8 i;

    if (intstat & SINT_STAT_RECV)                                 //receive data
    {
        this->dataLoopback(socketid);                            //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                              //connect successfully
    {
        if(this->keepAlive) 
        {
            WCHNET_SocketSetKeepLive(socketid, ENABLE);
        }

        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid], RECE_BUF_LEN);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                              //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
        //printf("TCP Connect Success\r\n");
        //printf("socket id: %d\r\n",socket[i]);
    }
    if (intstat & SINT_STAT_DISCONNECT)                           //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        //printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        //printf("TCP Timeout\r\n");
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void ethIF::handleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        //printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        //printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
        {    
            //printf("PHY Link Success\r\n");
        }
    }
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                this->handleSockInt(i, socketint);
        }
    }
}

void ethIF::mainTask()
{
    WCHNET_MainTask();

    /*Query the Ethernet global interrupt,
     * if there is an interrupt, call the global interrupt handler*/
    if(WCHNET_QueryGlobalInt())
    {
        this->handleGlobalInt();
    }
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
void ethIF::sendSrvPacket(u8 *buf, u32 len)
{
    if(this->SocketIdForListen != UINT8_MAX)
    {
        WCHNET_SocketSend(this->SocketIdForListen + 1  /*WTF         */, buf, &len);
    }

}

void ethIF::setSrvRetBuf(sRetBuf* newRetBuf)
{
    this->srvRetBuf= newRetBuf;
}

//
//uint8_t* ethIF::getRecvBuf(uint16_t* len)
//{
//    *len = this->bufLen;
//    return this->retBuf;
//}
//
//void ethIF::flushRecvBuf(void)
//{
//    this->bufLen = 0;
//}