/**
 * Integrated Interrupt Controller (changed to use TZIC)
 *
 * Interrupt documentation: i.MX53 reference manual chapter 3.2 p. 213
 *
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

//lib
#include <stdlib.h> //for malloc
#include <stdio.h> //for sprintf

//own includes
#include "interrupts.h"
#include "TZIC.h"
#include "kernelpanic.h"
#include "mxc_serial.h" //for cprintf
#include "trustzone.h" //for SMC_is_secure_world()

//the interrupt vector table would have to be in secure storage
static INTERRUPT_VECTOR interruptVectorTable[TZIC_TOTAL_IRQ];

//get least significant bit position
static int getLSB(unsigned int num){
  //avoid endless loop
  if(num == 0){
    return -1;
  }

  //get position of lowest bit
  unsigned int pos = 0;
  while((num & 1) == 0){
    ++pos;
    num>>=1;
  }
  return pos;
}

/**
 *	Global IRQ handler of this platform
 **/
void irqHandler(){
  unsigned int nIRQ = 0;

  //get interrupt number
  int reg = -1;
  int i;
  for(i = 0; reg == -1; ++i){
    reg = __REG(TZIC_HIPND0 + i*((4+TZIC_HIPND3-TZIC_HIPND0)/4));
    reg = getLSB(reg);
  }
  nIRQ = 32 * (i-1) + reg;

  //check
  if(nIRQ > 127){
    return;
  }
  
#ifdef DEBUG
  //log
  cprintf("IRQ: %d\n", nIRQ);
  
  //print pending interrupts before handler
  PrintPendingInterrupts();
#endif
  
  //call interrupt handler
  void* param = interruptVectorTable[nIRQ].pParam;
  interruptVectorTable[nIRQ].pfnHandler(nIRQ, param);

#ifdef DEBUG
  //print pending interrupts after handler
  PrintPendingInterrupts();
#endif
}

//function to acknowledge interrupts
void AcknowledgeInterrupt(const unsigned int nIRQ){

#ifdef DEBUG
  cprintf("ack IRQ %d\n", nIRQ);
#endif
  
  //acknowledge interrupt (=clear source register)
  const unsigned int mod = nIRQ % 32;
  const unsigned int div = nIRQ / 32;
  switch(div){
  case 0:
    __REG(TZIC_SRCCLEAR0) = (0x1 << mod);
    break;
  case 1:
    __REG(TZIC_SRCCLEAR1) = (0x1 << mod);
    break;
  case 2:
    __REG(TZIC_SRCCLEAR2) = (0x1 << mod);
    break;
  case 3:
    __REG(TZIC_SRCCLEAR3) = (0x1 << mod);
    break;
  default: ; //empty statement ";" after label cause declaration (char*) is not possible after a label
    //nIRQ > 127 => should never happen
    char* str = malloc(22); //string + strlen(UINT_MAX) = 12+10
    sprintf(str, "nIRQ %d > 127\n", nIRQ);
    kernelpanic(str);
  }
    
  //NOPs for pipelining
  __asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
}

static void stubHandler(unsigned int nIRQ, void *pParam) {
  //log
  cprintf("stubHandler IRQ %d\n", nIRQ);
  
  //acknowledge the interrupt, enable interrupts, return
  AcknowledgeInterrupt(nIRQ);
  EnableInterrupts();
  return;
}

void InitInterruptController(){
  //iMX53 doc: "Coming out of reset, all interrupts are configured
  // as secure, disabled, and set to priority level 0. Also, the
  // TZIC_INTCTRL register is set to disable all secure and
  // non-secure interrupts" (chapter 75.4.5 - p. 4601)

#ifdef TRUSTZONE
  //do not allow normal world to call this function (it resets the interrupt vector table!)
  if(!SMC_is_secure_world()){
    return;
  }
#endif
  
  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //init local data-structure
    for(int i = 0; i < TZIC_TOTAL_IRQ; i++) {
      interruptVectorTable[i].pfnHandler = stubHandler;
      interruptVectorTable[i].pParam     = (void*)0;
    }
  
    //steps to enable TZIC:
    // 1. set TZIC_INTSEC registers
    //    - all interrupts either secure or non-secure
    //    - all interrupts are secure at the beginning
    // => 1 = interrupt is IRQ, 0 = interrupt is FIQ
#ifdef TRUSTZONE
    __REG(TZIC_INTSEC0) = 0xFFFFFFFF;
    __REG(TZIC_INTSEC1) = 0xFFFFFCFF; //mark IRQ 40/41 (timers) as FIQ;
                                      //ALSO CHANGE IN RegisterInterrupt!
    __REG(TZIC_INTSEC2) = 0xFFFFFFFF;
    __REG(TZIC_INTSEC3) = 0xFFFFFFFF;
#else
    //TrustZone disabled => make all IRQ non-secure
    __REG(TZIC_INTSEC0) = 0xFFFFFFFF;
    __REG(TZIC_INTSEC1) = 0xFFFFFFFF;
    __REG(TZIC_INTSEC2) = 0xFFFFFFFF;
    __REG(TZIC_INTSEC3) = 0xFFFFFFFF;
#endif

#ifdef DEBUG
    cprintf("INTSEC:   %x %x %x %x\n",
            __REG(TZIC_INTSEC3), __REG(TZIC_INTSEC2),
            __REG(TZIC_INTSEC1), __REG(TZIC_INTSEC0));
#endif
  
    // 2. configure TZIC_ENSET/TZIC_ENCLEAR
    //    (interrupts not fully enabled until TZIC_INTCTRL set)
    // => enable all but the graphics interrupts (there is no screen attached, only serial cable!)
    __REG(TZIC_ENSET0)  = 0xFFFFFFFF;
    __REG(TZIC_ENSET1)  = 0xFFFFFFFF;
    __REG(TZIC_ENSET2)  = 0xFFDFFFFF; //mask OpenVG (GPU2D) busy signal irq
    __REG(TZIC_ENSET3)  = 0xFFFFFFAF; //mask VPU/GPU idle interrupt
#ifdef DEBUG
    cprintf("ENSET:    %x %x %x %x\n",
            __REG(TZIC_ENSET3), __REG(TZIC_ENSET2),
            __REG(TZIC_ENSET1), __REG(TZIC_ENSET0));
#endif
  
    // 3. configure TZIC_PRIORITY
    //    - each register holds 4 interrupt priorities
    //iterate over all priority registers
    for(int r = TZIC_PRIORITY0; r <= TZIC_PRIORITY31;
        r += ((4 + TZIC_PRIORITY31 - TZIC_PRIORITY0)/32)){
      //set all 4 interrupts to priority 1 (priority 0 is equal to
      //TZIC_PRIOMASK and would therefore be masked!)
      __REG(r) = 0x08080808;
    }
    //set INT 64 (timer interrupt) to highest priority
    //=> our scheduler should not be masked normally
    __REG(TZIC_PRIORITY16) = 0x08080800;

    // 4. set TZIC_PRIOMASK (allow interrupts to occur)
    // => set masked prio to "lowest" prio (= value 0xF8)
    __REG(TZIC_PRIOMASK) = 0x000000F8;
#ifdef DEBUG
    cprintf("PRIOMASK: %x\n", __REG(TZIC_PRIOMASK));
#endif

  
    // 5. set TZIC_INTCTRL (enable IC to send interrupts)
    // bit 31 = write non-secure enable bit
    // bit 16 = non-secure enable bit
    // bit 0  = secure enable bit
    // (now interrupts may occur)
    __REG(TZIC_INTCTRL) = 0x80010001;
#ifdef DEBUG
    cprintf("INTCTRL:  %x\n", __REG(TZIC_INTCTRL));
#endif

  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }
}

int RegisterInterrupt(const unsigned int nIRQ,
                      FN_INTERRUPT_HANDLER pfnHandler,
                      void *pParam){
#ifdef TRUSTZONE
  //do not allow normal world to call this function on non-secure interrupts
  //REMARK: INTSEC register does not work in normal world => we do not even know if the interrupt
  //is secure or not! => hard-code for now
  if((!SMC_is_secure_world()) && (nIRQ == 40 || nIRQ == 41)){
    return -1;
  }
#endif
  
  //check max IRQ
  if(nIRQ > 127){
    return -1;
  }
  
  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //add handler to vector table
    interruptVectorTable[nIRQ].pfnHandler = pfnHandler;
    interruptVectorTable[nIRQ].pParam     = pParam;
  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }
  
  return 0;
}

int EnableInterrupt(const unsigned int nIRQ){
  //normal world cannot set secure interrupts (FIQs), so this function does not need TZ checks
  
  //check
  if(nIRQ > 127){
    return -1;
  }
  
  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //enable interrupt number nIRQ in TZIC_ENSET
    const unsigned int mod = nIRQ % 32;
    const unsigned int div = nIRQ / 32;
    switch(div){
    case 0:
      __REG(TZIC_ENSET0) = __REG(TZIC_ENSET0) | (0x1 << mod);
      break;
    case 1:
      __REG(TZIC_ENSET1) = __REG(TZIC_ENSET1) | (0x1 << mod);
      break;
    case 2:
      __REG(TZIC_ENSET2) = __REG(TZIC_ENSET2) | (0x1 << mod);
      break;
    case 3:
      __REG(TZIC_ENSET3) = __REG(TZIC_ENSET3) | (0x1 << mod);
      break;
    default:
      return -1;
    }

#ifdef DEBUG
    cprintf("ENSET:    %x %x %x %x\n",
            __REG(TZIC_ENSET3), __REG(TZIC_ENSET2),
            __REG(TZIC_ENSET1), __REG(TZIC_ENSET0));
#endif
  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }

  
  return 0;
}

int DisableInterrupt(const unsigned int nIRQ) {
  //normal world cannot clear secure interrupts (FIQs), so this function does not need TZ checks
  
  //check
  if(nIRQ > 127){
    return -1;
  }

  //disable interrupt number nIRQ through TZIC_ENCLEAR
  const unsigned int mod = nIRQ % 32;
  const unsigned int div = nIRQ / 32;
  switch(div){
  case 0:
    __REG(TZIC_ENCLEAR0) = (0x1 << mod);
    break;
  case 1:
    __REG(TZIC_ENCLEAR1) = (0x1 << mod);
    break;
  case 2:
    __REG(TZIC_ENCLEAR2) = (0x1 << mod);
    break;
  case 3:
    __REG(TZIC_ENCLEAR3) = (0x1 << mod);
    break;
  default:
    return -1;
  }
  return 0;
}

int TriggerInterrupt(const unsigned int nIRQ){
  //normal world cannot trigger secure world interrupts, so this function does not need TZ checks
  
  //check
  if(nIRQ > 127){
    return -1;
  }

  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //set interrupt nIRQ
    const unsigned int mod = nIRQ % 32;
    const unsigned int div = nIRQ / 32;
    switch(div){
    case 0:
      __REG(TZIC_SRCSET0) = (0x1 << mod);
      break;
    case 1:
      __REG(TZIC_SRCSET1) = (0x1 << mod);
      break;
    case 2:
      __REG(TZIC_SRCSET2) = (0x1 << mod);
      break;
    case 3:
      __REG(TZIC_SRCSET3) = (0x1 << mod);
      break;
    default:
      return -1;
    }
  
    //software interrupt (will not be triggered unless enabled)
    //assert the interrupt (bit 31: 0 = assert, 1 = negate)
    __REG(TZIC_SWINT) = (0x000001FF & nIRQ);
    // READ ONLY REGISTER! no debug print here!
    // trigger secure interrupt from normal world will have no effect
  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }
  
  return 0;
}

void EnableInterrupts() {
  //enable FIQ if secure world, enable IRQ if normal world

#ifdef TRUSTZONE
  if(SMC_is_secure_world()){
    //secure world

    //enable all FIQs in the CPU's CPSR register
    __asm volatile(
      "STMDB	SP!, {r0}\n"
      "MRS r0, CPSR\n"
      "BIC r0, r0, #0x40\n" //clear FIQ bit
      "MSR CPSR_c, r0\n" // "_c" = set control bits
      "LDMIA	SP!, {r0}\n"
      );
  }else{
    //normal world
#endif
    
    //enable all IRQs in the CPU's CPSR register
    __asm volatile(
      "STMDB	SP!, {r0}\n"
      "MRS r0, CPSR\n"
      "BIC r0, r0, #0x80\n" //clear IRQ bit
      "MSR CPSR_c, r0\n" // "_c" = set control bits
      "LDMIA	SP!, {r0}\n"
      );
#ifdef TRUSTZONE
  }
#endif
}

void DisableInterrupts() {
  //enable/disable FIQ if secure world, enable/disable IRQ if normal world

#ifdef TRUSTZONE
  if(SMC_is_secure_world()){
    //secure world

    //disable all FIQs via the CPU's CPSR register
    __asm volatile(
      "STMDB	SP!, {r0}\n"
      "MRS r0, CPSR\n"
      "ORR r0, r0, #0x40\n" //set FIQ bit
      "MSR CPSR_c, r0\n" // "_c" = set control bits
      "LDMIA	SP!, {r0}\n"
      );
  }else{
    //normal world
#endif    
    //disable all IRQs via the CPU's CPSR register
    __asm volatile(
      "STMDB	SP!, {r0}\n"
      "MRS r0, CPSR\n"
      "ORR r0, r0, #0x80\n" //set IRQ bit
      "MSR CPSR_c, r0\n" // "_c" = set control bits
      "LDMIA	SP!, {r0}\n"
      );
#ifdef TRUSTZONE
  }
#endif
}

void ClearAllInterrupts(){
#ifdef TRUSTZONE
  //not applicable for normal world
  if(!SMC_is_secure_world()){
    return;
  }
#endif
  
  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //set clear bit for all interrupts
    __REG(TZIC_SRCCLEAR0) = 0xFFFFFFFF;
    __REG(TZIC_SRCCLEAR1) = 0xFFFFFFFF;
    __REG(TZIC_SRCCLEAR2) = 0xFFFFFFFF;
    __REG(TZIC_SRCCLEAR3) = 0xFFFFFFFF;
    
#ifdef DEBUG
    //print pending interrupts (should be none)
    PrintPendingInterrupts();
#endif
  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }
}

void PrintPendingInterrupts(){
  //disable interrupts if not already done
  const unsigned int enabled = CheckIRQ();
  if(enabled == 1){
    DisableInterrupts();
  }
  {
    //print pending interrupts
    cprintf("Pending IRQs:\n");
    cprintf("  HIPND:  %x %x %x %x\n",
            __REG(TZIC_HIPND3), __REG(TZIC_HIPND2),
            __REG(TZIC_HIPND1), __REG(TZIC_HIPND0));
    cprintf("  PND:    %x %x %x %x\n",
            __REG(TZIC_PND3), __REG(TZIC_PND2),
            __REG(TZIC_PND1), __REG(TZIC_PND0));
  }
  //enable interrupts again if they were enabled before
  if(enabled == 1){
    EnableInterrupts();
  }
}

//returns 0 (disabled) or 1 (enabled)
unsigned int CheckIRQ(){
  //get CPSR
  unsigned int reg = 0;
  __asm volatile("MRS %[Rd], CPSR" : [Rd] "=r" (reg));
  //7th bit = IRQ
  return (!((reg & 0b10000000) >> 7)) & 0x1;
}

//returns 0 (disabled) or 1 (enabled)
unsigned int CheckFIQ(){
  //get CPSR
  unsigned int reg = 0;
  __asm("MRS %[Rd], CPSR" : [Rd] "=r" (reg));
  //6th bit = FIQ
  return (!((reg & 0b1000000) >> 6)) & 0x1;
}
