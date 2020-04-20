/**
 * Wrapper file around TrustZone (assembly) functionality.
 *
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#include "trustzone.h"
#include "mxc_serial.h"


//check which mode we are in
MODE get_MODE(){
  //get CPSR (programm status register)
  unsigned int reg = 0;
  __asm volatile("MRS %[Rd], CPSR" : [Rd] "=r" (reg));
  //mode of operation = lowest 5 bits
  reg = reg & 0b11111;

  return (MODE)reg;
}

void print_MODE(){
  MODE m = get_MODE();
  switch(m){
  case USER:
    cprintf("USER mode\n");
    break;
  case FIQ:
    cprintf("FIQ mode\n");
    break;
  case IRQ:
    cprintf("IRQ mode\n");
    break;
  case SUPERVISOR:
    cprintf("SUPERVISOR mode\n");
    break;
  case ABORT:
    cprintf("ABORT mode\n");
    break;
  case UNDEF:
    cprintf("UNDEF mode\n");
    break;
  case SYSTEM:
    cprintf("SYSTEM mode\n");
    break;
  case MONITOR:
    cprintf("MONITOR mode\n");
    break;
  default:
    cprintf("ERROR: unknown mode: %d", (int)m);
  }
}

void print_SMC(unsigned int smc){
  cprintf("-- SMC %d --\n", smc);
}

//check which world we are in
void print_WORLD(){
#ifdef TRUSTZONE
  if(SMC_is_secure_world()){
    cprintf("Secure world\n");
  }else{
    cprintf("Normal world\n");
  }
#else
  cprintf("TrustZone disabled\n");
#endif
}

//check function for trustzone
unsigned int get_control_register(){
  /*
   * ARM1176JZF-S CPU:
   *   MRC p15, 0, <Rd>, c1, c0, 0 ; Read Control Register configuration data
   *   MCR p15, 0, <Rd>, c1, c0, 0 ; Write Control Register configuration data
   * Cortex-A8 CPU:
   *   MRC p15, 0, <Rd>, c1, c1, 0 ; Read Control Register configuration data
   *   MCR p15, 0, <Rd>, c1, c1, 0 ; Write Control Register configuration data
   * 
   *   c1:
   *     [31:7] SBZ - should be zero; unpredictable when read
   *     [6]    nET - early termination (not implemented in ARM1176JZF-S)
   *     [5]    AW  - determines if A bit in CPSR can be modified when in the Non-secure world
   *     [4]    FW  - determines if F bit in CPSR can be modified when in the Non-secure world
   *     [3]    EA  - external abort (see FIQ/IRQ)
   *     [2]    FIQ - FIQ behaviour: 0 = branch to FIQ mode; 1 = branch to Secure Monitor mode
   *     [1]    IRQ - IRQ behaviour: 0 = branch to IRQ mode; 1 = branch to Secure Monitor mode
   *     [0]    NS  - world of processor: 0 = secure; 1 = non-secure
   */
  unsigned int reg = 0;
#ifdef ARM1176JZF_S
  __asm volatile("MRC p15, 0, %[Rd], c1, c0, 0" : [Rd] "=r" (reg));
#elif CORTEX_A8
  __asm volatile("MRC p15, 0, %[Rd], c1, c1, 0" : [Rd] "=r" (reg));
#else
  #error "Unknown CPU"
#endif
  reg &= 127; //0b01111111

  return reg;
}

unsigned int SMC_is_secure_world(){
  //query monitor about world via monitor-call
  unsigned int world = 0;
  __asm volatile("SMC #2\n\t");
  __asm volatile("mov %0, r0" : "=r" (world));
  return !(world & 1);
}

unsigned int is_secure_world(){
  //check NS bit
  //=> NS == 1 means non-secure world or trustzone "off"
  //=> invert bit to show trustzone (NS = !trustzone)
  return !(get_control_register() & 1);
}
