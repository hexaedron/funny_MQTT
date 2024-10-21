#include <ch32v003fun.h>
#include "funny_defs.h"

#include "funny_time.h"


#include <stdbool.h>
#include <cstdlib>
#include <cstring>


#include "eth_driver.h"

//extern "C" {
#include "tcp_server.h"
//}

    #define RCC_PPRE1_DIV1                          ((uint32_t)0x00000000) /* PPRE1 not divided */
#define RCC_PPRE1_DIV2                          ((uint32_t)0x00000400) /* PPRE1 divided by 2 */
#define RCC_PPRE1_DIV4                          ((uint32_t)0x00000500) /* PPRE1 divided by 4 */

#define RCC_HPRE_DIV1                           ((uint32_t)0x00000000) /* SYSCLK not divided */
#define RCC_HPRE_DIV2                           ((uint32_t)0x00000080) /* SYSCLK divided by 2 */
#define RCC_HPRE_DIV4                           ((uint32_t)0x00000090) /* SYSCLK divided by 4 */
#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x1000) /* Time out for HSE start up */
static void SetSysClockTo120_HSE(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if((RCC->CTLR & RCC_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if(HSEStatus == (uint32_t)0x01)
    {
#if defined (CH32V20x_D8W)
        RCC->CFGR0 |= (uint32_t)(3<<22);
        /* HCLK = SYSCLK/2 */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2; 
        //RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV4;
#else
        /* HCLK = SYSCLK */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
#endif
        /* PCLK2 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
        /* PCLK1 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

        /*  CH32V20x_D6-PLL configuration: PLLCLK = HSE * 15 = 120 MHz (HSE=8MHZ)
         *  CH32V20x_D8-PLL configuration: PLLCLK = HSE/4 * 15 = 120 MHz (HSE=32MHZ)
         *  CH32V20x_D8W-PLL configuration: PLLCLK = HSE/2 * 15 = 240 MHz (HSE=32MHZ)
         */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_PLLSRC | RCC_PLLXTPRE |
                                              RCC_PLLMULL));

        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL15);

        /* Enable PLL */
        RCC->CTLR |= RCC_PLLON;
        /* Wait till PLL is ready */
        while((RCC->CTLR & RCC_PLLRDY) == 0)
        {
        }
        /* Select PLL as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
}

int main()
{
    //SystemInit();
//
//    RCC->CFGR0 |= (uint32_t)(3<<22);
//        /* HCLK = SYSCLK/2 */
//        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV4;
//     /* PCLK2 = HCLK */
//        RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
//        /* PCLK1 = HCLK */
//        RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;
//
    // Switch processor back to HSI so we don't eat dirt.
	//RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW) | RCC_SW_HSI;    

    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFF00FFFF;
    RCC->INTR = 0x009F0000; 
    SetSysClockTo120_HSE();


#ifdef WCH_FAST_INTERRUPT_ENABLED
	__set_INTSYSCR(0x3); // [Experimental] enable fast interrupt feature
#endif
	system_initSystick();

    //funGpioInitAll();

	
	u8 i;

    WCHNET_GetMacAddr(MACAddr);                                   //get the chip MAC address

    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);           //Ethernet library initialize
    //mStopIfError(i);
    if (i != WCHNET_ERR_SUCCESS)
    {
        while (true)
		{
			/* code */
		}
    }
		
#if KEEPALIVE_ENABLE                                               //Configure keep alive parameters
    {
        struct _KEEP_CFG cfg;

        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    WCHNET_CreateTcpSocketListen();                               //Create TCP Socket for Listening

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }
    }
}

