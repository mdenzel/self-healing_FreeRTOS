/**
 *	TrustZone related functionality.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef _TRUSTZONE_H_
#define _TRUSTZONE_H_

/*
 * ARM mode of operation
 */
typedef enum MODE {
  USER       = 0b10000, //usual ARM program (only unprivileged one)
  FIQ        = 0b10001, //for handling fast interrupts
  IRQ        = 0b10010, //for handling interrutps
  SUPERVISOR = 0b10011, //protected mode for OS
  ABORT      = 0b10111, //mode after data/prefetch abort
  UNDEF      = 0b11011, //on undefined instruction (e.g.
                        //   read TrustZone from normal world)
  SYSTEM     = 0b11111, //privileged user mode for OS
  MONITOR    = 0b10110  //secure mode for monitor
} MODE;

/*
 * Functions
 */
//ARM mode of operation
MODE get_MODE();
void print_MODE();
void print_SMC(unsigned int smc);
void print_WORLD();
//ARM TrustZone worlds
unsigned int get_control_register();
unsigned int SMC_is_secure_world(); //calls monitor and therefore also possible in normal world (but only after monitor is setup!)
unsigned int is_secure_world(); //throws undef. instruction exception if called from normal world!

#endif
