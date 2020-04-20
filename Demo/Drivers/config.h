/**
 * Offsets for the i.MX53 board (mostly original but with a few modifications).
 *
 * original author: Dongli Zhang
 * https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
 *
 * modifications:
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define AIPS1_BASE_ADDR		0x53F00000
#define AIPS2_BASE_ADDR		0x63F00000
#define CCM_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000D4000)

//input output multiplex controller
#define IOMUXC_BASE_ADDR    (AIPS1_BASE_ADDR + 0x000A8000)
#define IOMUXC_CSI0_DAT10   0xe8 /*enabling TXD_MUX*/
#define IOMUXC_CSI0_DAT11   0xec /*enabling RXD_MUX*/


#define UART_PHYS   0x53FBC000
/* Register definitions */
#define URXD  0x00 /* Receiver Register */
#define UTXD  0x40 /* Transmitter Register */
#define UCR1  0x80 /* Control Register 1 */
#define UCR2  0x84 /* Control Register 2 */
#define UCR3  0x88 /* Control Register 3 */
#define UCR4  0x8c /* Control Register 4 */
#define UFCR  0x90 /* FIFO Control Register */
#define USR1  0x94 /* Status Register 1 */
#define USR2  0x98 /* Status Register 2 */
#define UESC  0x9c /* Escape Character Register */
#define UTIM  0xa0 /* Escape Timer Register */
#define UBIR  0xa4 /* BRM Incremental Register */
#define UBMR  0xa8 /* BRM Modulator Register */
#define UBRC  0xac /* Baud Rate Count Register */
#define ONEMS 0xb0 /* One Millisecond Register */
#define UTS   0xb4 /* UART Test Register (mx31) */

/* UART Control Register Bit Fields.*/
#define  URXD_CHARRDY    (1<<15)
#define  URXD_ERR        (1<<14)
#define  URXD_OVRRUN     (1<<13)
#define  URXD_FRMERR     (1<<12)
#define  URXD_BRK        (1<<11)
#define  URXD_PRERR      (1<<10)
#define  URXD_RX_DATA    (0xFF)
#define  UCR1_ADEN       (1<<15) /* Auto dectect interrupt */
#define  UCR1_ADBR       (1<<14) /* Auto detect baud rate */
#define  UCR1_TRDYEN     (1<<13) /* Transmitter ready interrupt enable */
#define  UCR1_IDEN       (1<<12) /* Idle condition interrupt */
#define  UCR1_RRDYEN     (1<<9)  /* Recv ready interrupt enable */
#define  UCR1_RDMAEN     (1<<8)  /* Recv ready DMA enable */
#define  UCR1_IREN       (1<<7)  /* Infrared interface enable */
#define  UCR1_TXMPTYEN   (1<<6)  /* Transimitter empty interrupt enable */
#define  UCR1_RTSDEN     (1<<5)  /* RTS delta interrupt enable */
#define  UCR1_SNDBRK     (1<<4)  /* Send break */
#define  UCR1_TDMAEN     (1<<3)  /* Transmitter ready DMA enable */
#define  UCR1_UARTCLKEN  (1<<2)  /* UART clock enabled */
#define  UCR1_DOZE       (1<<1)  /* Doze */
#define  UCR1_UARTEN     (1<<0)  /* UART enabled */
#define  UCR2_ESCI		 (1<<15) /* Escape seq interrupt enable */
#define  UCR2_IRTS		 (1<<14) /* Ignore RTS pin */
#define  UCR2_CTSC		 (1<<13) /* CTS pin control */
#define  UCR2_CTS        (1<<12) /* Clear to send */
#define  UCR2_ESCEN      (1<<11) /* Escape enable */
#define  UCR2_PREN       (1<<8)  /* Parity enable */
#define  UCR2_PROE       (1<<7)  /* Parity odd/even */
#define  UCR2_STPB       (1<<6)  /* Stop */
#define  UCR2_WS         (1<<5)  /* Word size */
#define  UCR2_RTSEN      (1<<4)  /* Request to send interrupt enable */
#define  UCR2_TXEN       (1<<2)  /* Transmitter enabled */
#define  UCR2_RXEN       (1<<1)  /* Receiver enabled */
#define  UCR2_SRST		 (1<<0)  /* SW reset */
#define  UCR3_DTREN		 (1<<13) /* DTR interrupt enable */
#define  UCR3_PARERREN   (1<<12) /* Parity enable */
#define  UCR3_FRAERREN   (1<<11) /* Frame error interrupt enable */
#define  UCR3_DSR        (1<<10) /* Data set ready */
#define  UCR3_DCD        (1<<9)  /* Data carrier detect */
#define  UCR3_RI         (1<<8)  /* Ring indicator */
#define  UCR3_ADNIMP     (1<<7)  /* Autobaud Detection Not Improved */
#define  UCR3_RXDSEN     (1<<6)  /* Receive status interrupt enable */
#define  UCR3_AIRINTEN   (1<<5)  /* Async IR wake interrupt enable */
#define  UCR3_AWAKEN     (1<<4)  /* Async wake interrupt enable */
#define  UCR3_REF25		 (1<<3)  /* Ref freq 25 MHz */
#define  UCR3_REF30		 (1<<2)  /* Ref Freq 30 MHz */
#define  UCR3_INVT		 (1<<1)  /* Inverted Infrared transmission */
#define  UCR3_BPEN		 (1<<0)  /* Preset registers enable */
#define  UCR4_CTSTL_32   (32<<10) /* CTS trigger level (32 chars) */
#define  UCR4_INVR		 (1<<9)  /* Inverted infrared reception */
#define  UCR4_ENIRI		 (1<<8)  /* Serial infrared interrupt enable */
#define  UCR4_WKEN		 (1<<7)  /* Wake interrupt enable */
#define  UCR4_REF16		 (1<<6)  /* Ref freq 16 MHz */
#define  UCR4_IRSC		 (1<<5)  /* IR special case */
#define  UCR4_TCEN		 (1<<3)  /* Transmit complete interrupt enable */
#define  UCR4_BKEN		 (1<<2)  /* Break condition interrupt enable */
#define  UCR4_OREN		 (1<<1)  /* Receiver overrun interrupt enable */
#define  UCR4_DREN		 (1<<0)  /* Recv data ready interrupt enable */
#define  UFCR_RXTL_SHF   0       /* Receiver trigger level shift */
#define  UFCR_RFDIV      (7<<7)  /* Reference freq divider mask */
#define  UFCR_TXTL_SHF   10      /* Transmitter trigger level shift */
#define  USR1_PARITYERR  (1<<15) /* Parity error interrupt flag */
#define  USR1_RTSS		 (1<<14) /* RTS pin status */
#define  USR1_TRDY		 (1<<13) /* Transmitter ready interrupt/dma flag */
#define  USR1_RTSD		 (1<<12) /* RTS delta */
#define  USR1_ESCF		 (1<<11) /* Escape seq interrupt flag */
#define  USR1_FRAMEERR   (1<<10) /* Frame error interrupt flag */
#define  USR1_RRDY       (1<<9)  /* Receiver ready interrupt/dma flag */
#define  USR1_TIMEOUT    (1<<7)  /* Receive timeout interrupt status */
#define  USR1_RXDS		 (1<<6)  /* Receiver idle interrupt flag */
#define  USR1_AIRINT     (1<<5)  /* Async IR wake interrupt flag */
#define  USR1_AWAKE		 (1<<4)  /* Aysnc wake interrupt flag */
#define  USR2_ADET		 (1<<15) /* Auto baud rate detect complete */
#define  USR2_TXFE		 (1<<14) /* Transmit buffer FIFO empty */
#define  USR2_DTRF		 (1<<13) /* DTR edge interrupt flag */
#define  USR2_IDLE		 (1<<12) /* Idle condition */
#define  USR2_IRINT		 (1<<8)  /* Serial infrared interrupt flag */
#define  USR2_WAKE		 (1<<7)  /* Wake */
#define  USR2_RTSF		 (1<<4)  /* RTS edge interrupt flag */
#define  USR2_TXDC		 (1<<3)  /* Transmitter complete */
#define  USR2_BRCD		 (1<<2)  /* Break condition */
#define  USR2_ORE        (1<<1)  /* Overrun error */
#define  USR2_RDR        (1<<0)  /* Recv data ready */
#define  UTS_FRCPERR     (1<<13) /* Force parity error */
#define  UTS_LOOP        (1<<12) /* Loop tx and rx */
#define  UTS_TXEMPTY     (1<<6)  /* TxFIFO empty */
#define  UTS_RXEMPTY     (1<<5)  /* RxFIFO empty */
#define  UTS_TXFULL		 (1<<4)  /* TxFIFO full */
#define  UTS_RXFULL		 (1<<3)  /* RxFIFO full */
#define  UTS_SOFTRST     (1<<0)  /* Software reset */

#define __REG(x)    (*((volatile unsigned int *)(x)))

#endif
