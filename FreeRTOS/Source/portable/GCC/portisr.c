/**
 * FreeRTOS Interrupt Service Routines (ISR) (with modifications).
 *
 * original author: James Walmsley https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
 * 
 * modifications (mainly Timing functionality):
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

/*-----------------------------------------------------------
 * Components that can be compiled to either ARM or THUMB mode are
 * contained in port.c  The ISR routines, which can only be compiled
 * to ARM mode, are contained in this file.
 *----------------------------------------------------------*/

/*
	Changes from V2.5.2

	+ The critical section management functions have been changed.  These no
	  longer modify the stack and are safe to use at all optimisation levels.
	  The functions are now also the same for both ARM and THUMB modes.

	Changes from V2.6.0

	+ Removed the 'static' from the definition of vNonPreemptiveTick() to
	  allow the demo to link when using the cooperative scheduler.

	Changes from V3.2.4

	+ The assembler statements are now included in a single asm block rather
	  than each line having its own asm block.
*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "mxc_serial.h"
#include "interrupts.h"
#include "trustzone.h"

//optional includes
#ifdef TIMING
#include "timing.h"
#endif

/* Constants required to handle interrupts. */
#define portTIMER_MATCH_ISR_BIT		( ( uint8_t ) 0x01 )
#define portCLEAR_VIC_INTERRUPT		( ( uint32_t ) 0 )

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING		( ( uint32_t ) 0 )
volatile uint32_t ulCriticalNesting = 9999UL;

/*-----------------------------------------------------------*/

/* ISR to handle manual context switches (from a call to taskYIELD()). */
void vPortYieldProcessor( void ) __attribute__((interrupt("SWI"), naked));

/*
 * The scheduler can only be started from ARM mode, hence the inclusion of this
 * function here.
 */
void vPortISRStartFirstTask( void );
/*-----------------------------------------------------------*/

int g_bStarted = 0;

extern volatile void * volatile pxCurrentTCB;

void vPortISRStartFirstTask( void )
{
#ifdef DEBUG
  cprintf("\nvPortISRStartFirstTask: %x\n", *(StackType_t*)pxCurrentTCB);
#endif
  
  g_bStarted++;
    
  // Simply start the scheduler. This is included here as it can only be called from ARM mode.
  portRESTORE_CONTEXT();

  //should never reach here (= returns to main)
}
/*-----------------------------------------------------------*/

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 *
 * When a context switch is performed from the task level the saved task
 * context is made to look as if it occurred from within the tick ISR.  This
 * way the same restore context function can be used when restoring the context
 * saved from the ISR or that saved from a call to vPortYieldProcessor.
 */
void vPortYieldProcessor( void )
{
	/* Within an IRQ ISR the link register has an offset from the true return
	address, but an SWI ISR does not.  Add the offset manually so the same
	ISR return code can be used in both cases. */
    __asm volatile ( "ADD LR, LR, #4" ); //this is skipped in TrustZone because SMC 3 saved LR (and runs before vPortYieldProcessor) and SMC 0 (portSAVE_CONTEXT) will restore the saved LR

#ifdef DEBUG
    __asm volatile("push {r0-r12, lr}\n");
    cprintf("vPortYieldProcessor\n");
    __asm volatile("pop {r0-r12, lr}\n");
#endif

#ifdef TIMING
  __asm volatile("push {r0-r12, lr}\n");
  start_timer();
  __asm volatile("pop {r0-r12, lr}\n");
#endif
    
	/* Perform the context switch.  First save the context of the current task. */
	portSAVE_CONTEXT();

	/* Find the highest priority task that is ready to run. */
	__asm volatile ( "bl vTaskSwitchContext" );

	/* Restore the context of the new task. */
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

/*
 * The ISR used for the scheduler tick.
 */

/*-----------------------------------------------------------*/


/**
 *	This is the KERNEL's true entry point into an ISR.
 *
 *	We use the FreeRTOS to save context before entering the
 *	vectorising ISR, and emitting the true ISR event.
 *
 *	On return from the ISR we simply restore the context
 **/

extern void irqHandler(void);

void vFreeRTOS_ISR( void ) __attribute__((naked));
void vFreeRTOS_ISR( void ) {

#ifdef DEBUG
  //log
  __asm volatile("push {r0-r12, lr}\n");
  cprintf("\nvFreeRTOS_ISR\n");
  print_MODE();
  print_WORLD();
  cprintf("\n");
  __asm volatile("pop {r0-r12, lr}\n");
#endif

#ifdef TIMING
  __asm volatile("push {r0-r12, lr}\n");
  start_timer();
  __asm volatile("pop {r0-r12, lr}\n");
#endif
  
  //save last task
  portSAVE_CONTEXT();

#ifdef DEBUG
  cprintf("after portSAVE_CONTEXT\n");
  cprintf("IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif
  
  //interrupt handler (might schedule next task which is started below in portRESTORE_CONTEXT)
  irqHandler();

  //switch to next task
  portRESTORE_CONTEXT(); //XXX: here the context switch from vTickISR happens
}

/*
 * The interrupt management utilities can only be called from ARM mode.  When
 * THUMB_INTERWORK is defined the utilities are defined as functions here to
 * ensure a switch to ARM mode.  When THUMB_INTERWORK is not defined then
 * the utilities are defined as macros in portmacro.h - as per other ports.
 */
#ifdef THUMB_INTERWORK

	void vPortDisableInterruptsFromThumb( void ) __attribute__ ((naked));
	void vPortEnableInterruptsFromThumb( void ) __attribute__ ((naked));

	void vPortDisableInterruptsFromThumb( void )
	{
		__asm volatile (
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/
			"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.						*/
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/
			"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
			"BX		R14" );					/* Return back to thumb.					*/
	}

	void vPortEnableInterruptsFromThumb( void )
	{
		__asm volatile (
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/
			"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.							*/
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/
			"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
			"BX		R14" );					/* Return back to thumb.					*/
	}

#endif /* THUMB_INTERWORK */

/* The code generated by the GCC compiler uses the stack in different ways at
different optimisation levels.  The interrupt flags can therefore not always
be saved to the stack.  Instead the critical section nesting level is stored
in a variable, which is then saved as part of the stack context. */
void vPortEnterCritical( void )
{
	/* Disable interrupts as per portDISABLE_INTERRUPTS(); 							*/
	__asm volatile (
		"STMDB	SP!, {R0}			\n\t"	/* Push R0.								*/
		"MRS	R0, CPSR			\n\t"	/* Get CPSR.							*/
		"ORR	R0, R0, #0xC0		\n\t"	/* Disable IRQ, FIQ.					*/
		"MSR	CPSR, R0			\n\t"	/* Write back modified value.			*/
		"LDMIA	SP!, {R0}" );				/* Pop R0.								*/

	/* Now interrupts are disabled ulCriticalNesting can be accessed
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}

void vPortExitCritical( void )
{
	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as we are leaving a critical section. */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Enable interrupts as per portEXIT_CRITICAL().					*/
			__asm volatile (
				"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/
				"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/
				"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/ //FIXME: this enables IRQ+FIQ no matter what the state before was
				"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/
				"LDMIA	SP!, {R0}" );			/* Pop R0.						*/
		}
	}
}
