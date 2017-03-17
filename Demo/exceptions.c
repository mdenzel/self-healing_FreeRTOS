/**
 * Functionality to catch all possible exceptions.
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#include "exceptions.h"
#include "mxc_serial.h"
#include "kernelpanic.h"

//external functions
extern void irqHandler();

//secure world
void secure_undef(){
  kernelpanic("secure world undef\n");
}
void secure_swi(const unsigned int nIRQ){
  cprintf("secure world SWI %d\n", nIRQ);
}
void secure_pabort(){
  kernelpanic("secure world pabort\n");
}
void secure_dabort(){
  kernelpanic("secure world dabort\n");
}
void secure_irq(){
#ifdef DEBUG
  cprintf("secure world IRQ\n");
#endif
  irqHandler();
#ifdef DEBUG
  cprintf("secure world IRQ done\n");
#endif
}
void secure_fiq(){
#ifdef DEBUG
  cprintf("secure world FIQ\n");
#endif
  irqHandler();
#ifdef DEBUG
  cprintf("secure world FIQ done\n");
#endif
}

//normal world
void ns_reset(){
  cprintf("normal world reset\n");
}
void ns_undef(){
  kernelpanic("normal world Undef\n");
}
void ns_swi(const unsigned int nIRQ){
  cprintf("normal world SWI %d\n", nIRQ);
}
void ns_pa(){
  kernelpanic("normal world PA\n");
}
void ns_da(){
  kernelpanic("normal world DA\n");
}
void ns_irq(){
#ifdef DEBUG
  cprintf("normal world IRQ\n");
#endif
  irqHandler();
#ifdef DEBUG
  cprintf("normal world IRQ done\n");
#endif
}
