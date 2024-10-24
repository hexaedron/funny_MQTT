#include "tcpServer.h"

tcpServer::tcpServer(uint8_t* IPAddr, uint8_t* GWIPAddr, uint8_t* IPMask, uint16_t IPPort)
{
    WCHNET_GetMacAddr(this->MACAddr);
    this->IPAddr    = IPAddr;
    this->GWIPAddr  = GWIPAddr;
    this->IPMask    = IPMask;
    this->srcport   = IPPort;
}

tcpServer::~tcpServer()
{
}

void tcpServer::setIPAddr(uint8_t* addr)
{
    this->IPAddr = addr;
}

void tcpServer::setGWIPAddr(uint8_t* addr)
{
    this->GWIPAddr = addr;
}

void tcpServer::setIPMask(uint8_t* mask)
{
    this->IPMask = mask;
}

void tcpServer::setIPPort(uint16_t port)
{
    this->srcport = port;
}

void tcpServer::configKeepAlive(uint32_t KLIdle, uint32_t KLIntvl, uint32_t KLCount)
{
    this->keepAlive   = true;
    this->cfg.KLIdle  = KLIdle;
    this->cfg.KLIntvl = KLIntvl;
    this->cfg.KLCount = KLCount;
}

bool tcpServer::init(void)
{
    uint8_t i;
    
    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);           //Ethernet library initialize
    
    if (i != WCHNET_ERR_SUCCESS)
    {
        return false;
    }

    if(this->keepAlive)
    {
        WCHNET_ConfigKeepLive(&cfg);
    }
    
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    
    return this->CreateTcpSocketListen();
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
void tcpServer::mStopIfError(u8 iError)
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
void tcpServer::TIM2_Init(void)
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
bool tcpServer::CreateTcpSocketListen(void)
{
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = srcport;
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
void tcpServer::DataLoopback(u8 id)
{
#if 1
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;       //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {    //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);        //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                      //Clear sent data
    }
#else
    uint32_t len, totallen;
    uint8_t *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %d\r\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
#endif
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
void tcpServer::HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;

    if (intstat & SINT_STAT_RECV)                                 //receive data
    {
        this->DataLoopback(socketid);                            //Data loopback
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
void tcpServer::HandleGlobalInt(void)
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
                this->HandleSockInt(i, socketint);
        }
    }
}

void tcpServer::MainTask()
{
    WCHNET_MainTask();
}

uint8_t tcpServer::QueryGlobalInt()
{
    return WCHNET_QueryGlobalInt();
}