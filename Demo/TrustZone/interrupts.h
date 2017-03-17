/**
 * Tiny Interrupt Manager (with modifications)
 *
 * original author: James Walmsley <james@fullfat-fs.co.uk>
 * https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
 *
 * modifications:
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

typedef void (*FN_INTERRUPT_HANDLER)(unsigned int nIRQ, void *pParam);

typedef struct {
  FN_INTERRUPT_HANDLER pfnHandler; //function handling IRQn
  void* pParam; //special parameter to pass to the IRQ
} INTERRUPT_VECTOR;

void InitInterruptController ();

int RegisterInterrupt        (const unsigned int nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParam);
void AcknowledgeInterrupt    (const unsigned int nIRQ);
int EnableInterrupt          (const unsigned int nIRQ);
int DisableInterrupt         (const unsigned int nIRQ);
int TriggerInterrupt         (const unsigned int nIRQ);

void EnableInterrupts        ();
void DisableInterrupts       ();
void ClearAllInterrupts      ();
void PrintPendingInterrupts  ();

unsigned int CheckIRQ        ();
unsigned int CheckFIQ        ();

#endif
