/**
 * Printing functionality (via serial cable).
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 *
 * XXX: file is based on code from (but then changed quite heavily)
 *      Dongli Zhang https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
 **/

//own
#include "config.h"
#include "mxc_serial.h"
//C includes
#include <stdio.h> //for vsnprintf
#include <stdarg.h> //for var args

// set the iomuxc policy to enable serial port I/O
void iomuxc_init(void){
  //enable ALT2 in CSI0_DAT10 for TXD_MUX
  __REG(IOMUXC_BASE_ADDR + IOMUXC_CSI0_DAT10) =  0b00010; //SION=0, ALT2

  //enable ALT2 in CSI0_DAT11 for RXD_MUX
  __REG(IOMUXC_BASE_ADDR + IOMUXC_CSI0_DAT11) =  0b00010; //SION=0, ALT2

  //daisy chain setup
  // IOMUXC_UART1_IPP_UART_RXD_MUX_SELECT_INPUT = 0x878
  __REG(IOMUXC_BASE_ADDR + 0x878) = 0x1;
}

static void mxc_serial_putc(const char c){
  //write character
  __REG(UART_PHYS + UTXD) = c;

  //wait until character was consumed
  while(!(__REG(UART_PHYS + UTS) & UTS_TXEMPTY));
  
  if(c == '\n'){
    mxc_serial_putc('\r');
  }
}

static int mxc_serial_getc(char* const c){
  //check if there is something to read
  if(__REG(UART_PHYS + UTS) & UTS_RXEMPTY){
    return -1;
  }

  //get value and check for errors
  int r = __REG(UART_PHYS + URXD);
  if((r & 0xFC00) != 0x8000){ //ready bit should be set
    return r;
  }

  //read character
  *c = (char)(r & 0xFF);
  return 0;
}

unsigned int readline(char* const buf, const unsigned int size){
  //variables
  unsigned int i = 0;
  int err = 0;

  //while buf still has space and no error
  while(i < size-1 && err == 0){
    //get char
    err = mxc_serial_getc(&buf[i]);
    
    //check errors
    if(err == 0){
      //stop at newline
      if(buf[i] == '\n' || buf[i] == '\r'){
        break;
      }
      
      //next value
      ++i;
    }
    else if(err == -1){
      //empty input
      if(i == 0){
        //nothing read yet => stop
        break;
      }else{
        //some characters were already read => wait for the rest of them
        err = 0;
        continue;
      }
    }else{
      //serious error => stop reading
      cprintf("getc error: %x\n", err);
      break;
    }
  }

  buf[i] = '\0';
  return i;
}

void mxc_serial_init(void){
  //wait until UART transmission is empty
  //while(!(__REG(UART_PHYS + UTS) & UTS_TXEMPTY));
  
  //disable UART
  __REG(UART_PHYS + UCR1) &= ~UCR1_UARTEN;
  //reset
  __REG(UART_PHYS + UCR1) = 0x0;
  __REG(UART_PHYS + UCR2) = 0x0;
  while(!(__REG(UART_PHYS + UCR2) & UCR2_SRST)); //wait for software reset (SRST)

  //enable uart
  __REG(UART_PHYS + UCR1) = UCR1_UARTEN;

  // set to 8N1 (no parity)
  __REG(UART_PHYS + UCR2) &= ~UCR2_PREN; //disable parity check "N"
  __REG(UART_PHYS + UCR2) |= UCR2_WS;    //word size 8
  __REG(UART_PHYS + UCR2) &= ~UCR2_STPB; //1 stop bit
  //ignore RTS (req to send => always just send)
  __REG(UART_PHYS + UCR2) |= UCR2_IRTS;
  //set CTS control not needed
  //enable FIFOs = enable read; enable write; no reset
  __REG(UART_PHYS + UCR2) |= UCR2_TXEN | UCR2_RXEN | UCR2_SRST;
  
  //DSR|DCD|RI; RXDMUXSEL (must be 1)
  __REG(UART_PHYS + UCR3) = 0x0704;
  //CTS trigger level does not matter
  __REG(UART_PHYS + UCR4) = 0x8000;
  //uart escape character (2b = '+')
  __REG(UART_PHYS + UESC) = 0x002b;
  //escape timer max 2ms
  __REG(UART_PHYS + UTIM) = 0x0;
  //reset UTS
  __REG(UART_PHYS + UTS)  = 0x0060; //TXEMPTY and RXEMPTY = 1

  //set frequency
  const unsigned int clk = 33300000; //33.3MHz
  __REG(UART_PHYS + UFCR)  = 0x0A1E; //TxFIFO int <2 chars; divide input clock by 2 (RFDIV 0b100 = RFDIV 2); RxFIFO int 30 chars
  __REG(UART_PHYS + ONEMS) = clk / 2 / 1000; // frequency / RFDIV / 1000
  __REG(UART_PHYS + UBIR)  = 0xf; //clock 33.3 MHz => baudrate = clk / (16 * (UBMR+1) / (UBIR+1))
  __REG(UART_PHYS + UBMR)  = 0x121;

  //clear status flags
  __REG(UART_PHYS + USR2) |= USR2_ADET | USR2_IDLE | USR2_IRINT | USR2_WAKE | USR2_RTSF;
  __REG(UART_PHYS + USR2) |= USR2_BRCD | USR2_ORE | USR2_RDR;

  //enable controller (XXX: FIFO Interrupts would be IRQ 31)
  __REG(UART_PHYS + UCR1) = UCR1_UARTEN;
  
  return;
}

void cprintf(char *fmt, ...)
{
  const unsigned int len = 256;
  char s[len];
  va_list ap;

  //get var args and print them to s
  va_start(ap, fmt);
  vsnprintf(s, len-1, fmt, ap);
  va_end(ap);

  //print s
  for(unsigned int i = 0; i < len; ++i){
    if(s[i] == '\0'){
      break;
    }
    mxc_serial_putc(s[i]);
  }
}
