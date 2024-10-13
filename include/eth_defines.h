#define R32_ETH_MIWR            (*((volatile uint32_t *)(0x40028000+0x24)))
#define R8_ETH_MIREGADR         (*((volatile uint8_t *)(0x40028000+0x24))) /* MII address register*/
#define  RB_ETH_MIREGADR_MASK   0x1F                  /* RW PHY register address mask */
#define R8_ETH_MISTAT           (*((volatile uint8_t *)(0x40028000+0x25))) /* RW PHY register address mask */
//#define  RB_ETH_MIREGADR_MIIWR  0x20                  /* WO MII write command */
#define R16_ETH_MIWR            (*((volatile uint16_t *)(0x40028000+0x26))) /* WO MII Write Data Register */
#define R32_ETH_MAADRL          (*((volatile uint32_t *)(0x40028000+0x28))) /* RW MAC 1-4 */
#define R8_ETH_MAADRL1          (*((volatile uint8_t *)(0x40028000+0x28))) /* RW MAC 1 */
#define R8_ETH_MAADRL2          (*((volatile uint8_t *)(0x40028000+0x29))) /* RW MAC 2 */
#define R8_ETH_MAADRL3          (*((volatile uint8_t *)(0x40028000+0x2A))) /* RW MAC 3 */
#define R8_ETH_MAADRL4          (*((volatile uint8_t *)(0x40028000+0x2B))) /* RW MAC 4 */
#define R16_ETH_MAADRH          (*((volatile uint16_t *)(0x40028000+0x2C))) /* RW MAC 5-6 */
#define R8_ETH_MAADRL5          (*((volatile uint8_t *)(0x40028000+0x2C))) /* RW MAC 4 */
#define R8_ETH_MAADRL6          (*((volatile uint8_t *)(0x40028000+0x2D))) /* RW MAC 4 */

#define R32_ETH_TIM             (*((volatile uint32_t *)(0x40028000+0x1C)))
#define R16_ETH_EPAUS           (*((volatile uint16_t *)(0x40028000+0x1C))) /* RW Flow Control Pause Frame Time Register */
#define R16_ETH_MAMXFL          (*((volatile uint16_t *)(0x40028000+0x1E))) /* RW Maximum Received Packet Length Register */
#define R16_ETH_MIRD            (*((volatile uint16_t *)(0x40028000+0x20))) /* RW MII read data register */

//PHY address
#define PHY_BMCR                0x00                                            /* Control Register */
#define PHY_BMSR                0x01                                            /* Status Register */
#define PHY_ANAR                0x04                                            /* Auto-Negotiation Advertisement Register */
#define PHY_ANLPAR              0x05                                            /* Auto-Negotiation Link Partner Base  Page Ability Register*/
#define PHY_ANER                0x06                                            /* Auto-Negotiation Expansion Register */
#define PHY_MDIX                0x1e                                            /* Custom MDIX Mode Register */
//Custom MDIX Mode Register  @PHY_MDIX
#define PN_NORMAL               0x04                                            /* Analog p, n polarity selection */
#define MDIX_MODE_MASK          0x03                                            /* mdix settings */
#define MDIX_MODE_AUTO          0x00                                            /*  */
#define MDIX_MODE_MDIX          0x01
#define MDIX_MODE_MDI           0x02
//ECON2 test mode, to be determined
#define RX_VCM_MODE_0
#define RX_VCM_MODE_1
#define RX_VCM_MODE_2
#define RX_VCM_MODE_3
//RX reference voltage value setting  @RX_REF
#define RX_REF_25mV             (0<<2)                                          /* 25mV */
#define RX_REF_49mV             (1<<2)                                          /* 49mV */
#define RX_REF_74mV             (2<<2)                                          /* 74mV */
#define RX_REF_98mV             (3<<2)                                          /* 98mV */
#define RX_REF_123mV            (4<<2)                                          /* 123mV */
#define RX_REF_148mV            (5<<2)                                          /* 148mV */
#define RX_REF_173mV            (6<<2)                                          /* 173mV */
#define RX_REF_198mV            (7<<2)                                          /* 198mV */
//TX DRIVER Bias Current  @TX_AMP
#define TX_AMP_0                (0<<0)                                          /* 43mA   / 14.5mA   (1.4V/0.7V) */
#define TX_AMP_1                (1<<0)                                          /* 53.1mA / 18mA     (1.8V/0.9V) */
#define TX_AMP_2                (2<<0)                                          /* 75.6mA / 25.6mA   (2.6V/1.3V) */
#define TX_AMP_3                (3<<0)                                          /* 122mA  / 41.45mA  (4.1V/2.3V) */
//FCEN pause frame control      @FCEN
#define FCEN_0_TIMER            (3<<4)                                          /* Send a 0 timer pause frame, then stop sending */
#define FCEN_CYCLE              (2<<4)                                          /* Periodically send pause frames */
#define FCEN_ONCE               (1<<4)                                          /* Send pause frame once, then stop sending */
#define FCEN_STOP               (0<<4)                                          /* Stop sending pause frames */
//PADCFG short packet control  @PADCFG
#define PADCFG_AUTO_0           (7<<5)                                          /* All short packets are filled with 00h to 64 bytes, then 4 bytes crc */
#define PADCFG_NO_ACT_0         (6<<5)                                          /* No padding for short packets */
/* The detected VLAN network packet whose field is 8100h is automatically filled
with 00h to 64 bytes, otherwise the short packet is filled with 60 bytes of 0, and then 4 bytes of crc after filling */
#define PADCFG_DETE_AUTO        (5<<5)
#define PADCFG_NO_ACT_1         (4<<5)                                          /* No padding for short packets */
#define PADCFG_AUTO_1           (3<<5)                                          /* All short packets are filled with 00h to 64 bytes, then 4 bytes crc */
#define PADCFG_NO_ACT_2         (2<<5)                                          /* No padding for short packets */
#define PADCFG_AUTO_3           (1<<5)                                          /* All short packets are filled with 00h to 60 bytes, and then 4 bytes crc */
#define PADCFG_NO_ACT_3         (0<<5)                                          /* No padding for short packets */


//MII control
#define  RB_ETH_MIREGADR_MIIWR  0x20                                            /* WO MII write command */
#define  RB_ETH_MIREGADR_MIRDL  0x1f                                            /* RW PHY register address */

/* Bit or field definition for PHY basic status register */
#define PHY_Linked_Status       ((uint16_t)0x0004)      /* Valid link established */

#define PHY_Reset                               ((uint16_t)0x8000)      /* PHY Reset */

#define PHY_AutoNego_Complete                   ((uint16_t)0x0020)      /* Auto-Negotioation process completed */