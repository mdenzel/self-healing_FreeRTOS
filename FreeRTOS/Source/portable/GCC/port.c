/**
 * Raspberry Pi Porting layer for FreeRTOS. (with modifications)
 *
 * original author: James Walmsley https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
 * 
 * modifications (mainly TrustZone and EPIT timer functionality):
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#include <stdlib.h>

//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

//TrustZone
#include "interrupts.h"
#include "kernelpanic.h"

//Drivers
#include "mxc_serial.h"

/* Constants required to setup the task context. */
#define portINITIAL_SPSR				( ( StackType_t ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT				( ( StackType_t ) 0x20 )
#define portINSTRUCTION_SIZE			( ( StackType_t ) 4 )
#define portNO_CRITICAL_SECTION_NESTING	( ( StackType_t ) 0 )

// Constants required to setup the VIC for the tick ISR.
//EPIT timer 1 at 0x53FAC000
//EPIT timer 2 at 0x53FB0000
#define portTIMER_BASE ((unsigned long) 0x53FAC000)

typedef struct EPIT_TIMER_REGS {
  unsigned int CR;   //control register
  unsigned int SR;   //status register
  unsigned int LR;   //load register
  unsigned int CMPR; //compare register
  unsigned int CNR;  //counter register
} EPIT_TIMER_REGS;

static volatile EPIT_TIMER_REGS* const pRegs = (EPIT_TIMER_REGS*) (portTIMER_BASE);

/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );

/* 
 * The scheduler can only be started from ARM mode, so 
 * vPortISRStartFirstSTask() is defined in portISR.c. 
 */
extern void vPortISRStartFirstTask( void );

/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
StackType_t *pxOriginalTOS;

	pxOriginalTOS = pxTopOfStack;
    
	/* To ensure asserts in tasks.c don't fail, although in this case the assert
	is not really required. */
	pxTopOfStack--;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;		
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) 0xaaaaaaaa;	/* R14 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x01010101;	/* R1 */
	pxTopOfStack--;	

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
	*pxTopOfStack = ( StackType_t ) portINITIAL_SPSR;

	if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00 )
	{
		/* We want the task to start in thumb mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}

	pxTopOfStack--;

	/* Some optimisation levels use the stack differently to others.  This 
	means the interrupt flags cannot always be stored on the stack and will
	instead be stored in a variable, which is then saved as part of the
	tasks context. */
	*pxTopOfStack = portNO_CRITICAL_SECTION_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	vPortISRStartFirstTask();	

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/

/*
 *	This is the TICK interrupt service routine, note. no SAVE/RESTORE_CONTEXT here
 *	as thats done in the bottom-half of the ISR.
 */
void vTickISR(unsigned int nIRQ, void *pParam )
{
#ifdef DEBUG
  //log
  cprintf("tick IRQ %d\n", nIRQ);
#endif

  //acknowledge IRQ and clear status register
  AcknowledgeInterrupt(nIRQ);
  pRegs->SR = 0b1;

  //count
  xTaskIncrementTick();

#if configUSE_PREEMPTION == 1
  vTaskSwitchContext();
#endif
}

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
  //setup rate
  const uint32_t clock_frequency = 32768; //Hz
  const uint32_t ulCompareMatch = clock_frequency * configTICK_RATE_HZ;

  //log
  cprintf("setup timer interrupt\n");
  
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //FIXME: for a secure implementation this would have to be shielded from normal world!
    /*EPIT documentation: i.MX53 spec p. 1140
     *  bit  name   needed(now/runtime)      description
     *  -----------------------------------------------------------
     *  0     EN         0/1      enable bit
     *  1     ENMOD      0        enable mod (0=restart from same
     *                              value, 1=reset on restart)
     *  2     OCIEN      1        enable/disable interrupt
     *  3     RLD        1        reload control (0=free,
     *                              1=setup-forget)
     *  4-15  PRESCALAR  0x000    clock prescaler val (divide by X)
     *  16    SWR        0        software reset
     *  17    IOVW       1/0      counter overwrite (Load Reg LR)
     *  18    DBGEN      1        functional in dbg mode?
     *  19    WAITEN     1        functional in wait mode?
     *  20    reserved   0
     *  21    STOPEN     0        functional in stop mode?
     *  22-23 OM         0b00     output mode (set pin on match?
     *                              => not needed due to IRQ)
     *  24-25 CLKSRC     0b01     clock source (00 off,
     *                              01 peripheral, 10 high freq.
     *                              clock, 11 low freq. clock)
     *  26-31 reserved   0
     *
     * "Peripheral bus write access to the EPIT control register (EPITCR) and the EPIT load
     * register (EPITLR) results in one cycle of wait state, while other valid peripheral bus
     * accesses are with 0 wait state."
     * => NOP needed after CR and LR
     */
#ifdef DEBUG
    cprintf("EPIT: %x (before init)\n", pRegs->CR);
#endif
    
    //1. make sure EPIT is disabled
    pRegs->CR = pRegs->CR & (~(0b1));
    __asm volatile("nop");
    
    //2. set OM=00
    pRegs->CR = pRegs->CR & (~(0b11 << 22));
    __asm volatile("nop");
    
    //3. disable EPIT interrupts
    pRegs->CR = pRegs->CR & (~(0b100));
    __asm volatile("nop");
    
    //4. program CLKSRC to 0b10
    pRegs->CR = (pRegs->CR & (~(0b1 << 24))) | (0b1 << 25);
    __asm volatile("nop");

    //5. clear EPIT status register (SR) => write 0b1 to clear!
    pRegs->SR = 0b1;
    
    //6. enable EPIT interrupts
    pRegs->CR = pRegs->CR | 0b100;
    __asm volatile("nop");
    
    //7. bring counter to defined state (IOVW and load register LR)
    //XXX: setting IOVW did not work, thus ENMOD is used (step 8.)
    //pRegs->CR = pRegs->CR | (0b1 << 17);
    //__asm volatile("nop");
    //init load/compare values
    pRegs->LR   = ulCompareMatch;
    __asm volatile("nop");
    pRegs->CMPR = ulCompareMatch;
    //reset IOVW again
    //pRegs->CR = pRegs->CR & (~(0b1 << 17));
    //__asm volatile("nop");

    
    //8. init rest (except enable bit EN)
    // 0b 0000 0001 0000 1100 0000 0000 0000 1110
    // 0x 0    1    0    C    0    0    0    E
    pRegs->CR = 0x010C000E;
    __asm volatile("nop");
    
    //9. enable EPIT
    pRegs->CR = pRegs->CR | (0b1);
    __asm volatile("nop");
    //CR should now be 0x...F
    
#ifdef DEBUG
    cprintf("EPIT: %x (=0x010C000F?)\n", pRegs->CR);
    cprintf("EPIT CNR:  %x\n", pRegs->CNR);
    cprintf("EPIT CMPR: %x\n", pRegs->CMPR);
    cprintf("EPIT LR:   %x\n", pRegs->LR);
    cprintf("EPIT CNR:  %x\n", pRegs->CNR);
#endif
    
    //setup timer interrupt
    //EPIT1 = IRQ 40
    //EPIT2 = IRQ 41
    if(RegisterInterrupt(40, vTickISR, NULL) != 0){
      kernelpanic("EPIT: could not register IRQ 40");
    }
    if(EnableInterrupt(40) != 0){ //all interrupts enabled by default, so this is redundant for maybe later
      kernelpanic("EPIT: IRQ 40 cannot be enabled");
    }
  }
  if(enabled == 1){
    EnableInterrupts();
  }
}
/*-----------------------------------------------------------*/

