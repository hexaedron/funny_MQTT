#define KEEPALIVE_ENABLE                1                //Enable keep alive function

u8 MACAddr[6];                                          //MAC address
u8 IPAddr[4] = { 192, 168, 1, 33 };                     //IP address
u8 GWIPAddr[4] = { 192, 168, 1, 1 };                    //Gateway IP address
u8 IPMask[4] = { 255, 255, 255, 0 };                    //subnet mask
u16 srcport = 1000;                                     //source port

u8 SocketIdForListen;                                   //Socket for Listening
u8 socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];

///===================== MESSY =================================
///=============================================================
///=============================================================
///=============================================================
void printf(const char* x, ...)
{}
///=============================================================
///=============================================================
///=============================================================
///=============================================================

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    //TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    // Enable TIM2
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
    // Reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;

    //TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;


    //TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    //TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	
    //TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   


	TIM2->CTLR1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS)));
	TIM2->CTLR1 |= TIM_CounterMode_Up;

	TIM2->CTLR1 &= ~(TIM_CTLR1_CKD);

	TIM2->ATRLR = (FUNCONF_SYSTEM_CORE_CLOCK / 1000000);
    TIM2->PSC = (uint16_t)(WCHNETTIMERPERIOD * 1000 - 1);

    //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM2->DMAINTENR |= TIM_IT_Update;

	//TIM_Cmd(TIM2, ENABLE);
	TIM2->CTLR1 |= TIM_CEN;

    //TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
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
void WCHNET_CreateTcpSocketListen(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    i = WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
    printf("SocketIdForListen %d\r\n", SocketIdForListen);
    mStopIfError(i);
    i = WCHNET_SocketListen(SocketIdForListen);                   //listen for connections
    mStopIfError(i);
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
void WCHNET_DataLoopback(u8 id)
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
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;

    if (intstat & SINT_STAT_RECV)                                 //receive data
    {
        WCHNET_DataLoopback(socketid);                            //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                              //connect successfully
    {
#if KEEPALIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid],
        RECE_BUF_LEN);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                              //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
        printf("TCP Connect Success\r\n");
        printf("socket id: %d\r\n",socket[i]);
    }
    if (intstat & SINT_STAT_DISCONNECT)                           //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Timeout\r\n");
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}