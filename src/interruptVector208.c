#include <ch32v003fun.h>

void InterruptVector()     __attribute__((naked)) __attribute((section(".init"))) __attribute((alias("InterruptVector208"))) __attribute((naked));
void InterruptVector208()  __attribute__((naked)) __attribute((section(".init"))) __attribute((naked));
//void handle_reset( void );

void DefaultIRQHandler( void ) __attribute__((section(".text.vector_handler"))) __attribute__((naked)) __attribute__((used));
//void USBFSWakeUp_IRQHandler( void )		__attribute__((section(".text.vector_handler"))) __attribute((weak,alias("DefaultIRQHandler"))) __attribute__((used));
//void BB_IRQHandler( void ) 			 	 __attribute__((section(".text.vector_handler"))) __attribute((weak,alias("DefaultIRQHandler"))) __attribute__((used));
//void LLE_IRQHandler( void ) 			 __attribute__((section(".text.vector_handler"))) __attribute((weak,alias("DefaultIRQHandler"))) __attribute__((used));

void InterruptVector208()
{
	asm volatile( "\n\
	.align	1 \n\
	.option norvc; \n\
	j handle_reset \n"
"	.word   0 \n\
	.word   NMI_Handler                /* NMI */ \n\
	.word   HardFault_Handler          /* Hard Fault */ \n\
	.word   0 \n"

"	.word   Ecall_M_Mode_Handler       /* Ecall M Mode */ \n\
	.word   0 \n\
	.word   0 \n\
	.word   Ecall_U_Mode_Handler       /* Ecall U Mode */ \n\
	.word   Break_Point_Handler        /* Break Point */ \n\
	.word   0 \n\
	.word   0 \n"

"	.word   SysTick_Handler            /* SysTick */ \n \
	.word   0 \n \
	.word   SW_Handler                 /* SW */ \n \
	.word   0 \n \
	/* External Interrupts */ \n \
	.word   WWDG_IRQHandler            /* Window Watchdog */ \n \
	.word   PVD_IRQHandler             /* PVD through EXTI Line detect */ \n \
	.word   TAMPER_IRQHandler          /* TAMPER */ \n \
	.word   RTC_IRQHandler             /* RTC */ \n \
	.word   FLASH_IRQHandler           /* Flash */ \n \
	.word   RCC_IRQHandler             /* RCC */ \n \
	.word   EXTI0_IRQHandler           /* EXTI Line 0 */ \n \
	.word   EXTI1_IRQHandler           /* EXTI Line 1 */ \n \
	.word   EXTI2_IRQHandler           /* EXTI Line 2 */ \n \
	.word   EXTI3_IRQHandler           /* EXTI Line 3 */ \n \
	.word   EXTI4_IRQHandler           /* EXTI Line 4 */ \n \
	.word   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */ \n \
	.word   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */ \n \
	.word   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */ \n \
	.word   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */ \n \
	.word   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */ \n \
	.word   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */ \n \
	.word   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */ \n \
	.word   ADC1_2_IRQHandler          /* ADC1_2 */ \n \
	.word   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */ \n \
	.word   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */ \n \
	.word   CAN1_RX1_IRQHandler        /* CAN1 RX1 */ \n \
	.word   CAN1_SCE_IRQHandler        /* CAN1 SCE */ \n \
	.word   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */ \n \
	.word   TIM1_BRK_IRQHandler        /* TIM1 Break */ \n \
	.word   TIM1_UP_IRQHandler         /* TIM1 Update */ \n \
	.word   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */ \n \
	.word   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */ \n \
	.word   TIM2_IRQHandler            /* TIM2 */ \n \
	.word   TIM3_IRQHandler            /* TIM3 */ \n \
	.word   TIM4_IRQHandler            /* TIM4 */ \n \
	.word   I2C1_EV_IRQHandler         /* I2C1 Event */ \n \
	.word   I2C1_ER_IRQHandler         /* I2C1 Error */ \n \
	.word   I2C2_EV_IRQHandler         /* I2C2 Event */ \n \
	.word   I2C2_ER_IRQHandler         /* I2C2 Error */ \n \
	.word   SPI1_IRQHandler            /* SPI1 */ \n \
	.word   SPI2_IRQHandler            /* SPI2 */ \n \
	.word   USART1_IRQHandler          /* USART1 */ \n \
	.word   USART2_IRQHandler          /* USART2 */ \n \
	.word   USART3_IRQHandler          /* USART3 */ \n \
	.word   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */ \n \
	.word   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */ \n \
	.word   USBWakeUp_IRQHandler       /* USB Wake up from suspend */ \n \
	.word   0                           /* USBFS Break */ \n \
	.word   0                           /* USBFS Wake up from suspend */ \n "
#if defined(CH32V20x_D6) //CH32V20x variants
"	.word   UART4_IRQHandler           /* UART4 */ \n \
	.word   DMA1_Channel8_IRQHandler   /* DMA1 Channel8 */ \n"
#elif defined(CH32V20x_D8)
"	.word   ETH_IRQHandler             /* ETH global */  \n\
	.word   ETHWakeUp_IRQHandler       /* ETH Wake up */  \n\
	.word	0 \n\
	.word	0 \n\
	.word   TIM5_IRQHandler            /* TIM5 */  \n\
	.word   UART4_IRQHandler           /* UART4 */  \n\
	.word   DMA1_Channel8_IRQHandler   /* DMA1 Channel8 */  \n\
	.word   OSC32KCal_IRQHandler       /* OSC32KCal */  \n\
	.word   OSCWakeUp_IRQHandler       /* OSC Wake Up */ \n"
#elif defined(CH32V20x_D8W)
"	.word   ETH_IRQHandler             /* ETH global */  \n\
	.word   ETHWakeUp_IRQHandler       /* ETH Wake up */  \n\
	.word   0                           /* BLE BB */  \n\
	.word   0                           /* BLE LLE */  \n\
	.word   TIM5_IRQHandler            /* TIM5 */  \n\
	.word   UART4_IRQHandler           /* UART4 */  \n\
	.word   DMA1_Channel8_IRQHandler   /* DMA1 Channel8 */  \n\
	.word   OSC32KCal_IRQHandler       /* OSC32KCal */  \n\
	.word   OSCWakeUp_IRQHandler       /* OSC Wake Up */ \n "
#endif //CH32V20x variants
//defined(CH32V20x)

"	.option rvc; \n");
}