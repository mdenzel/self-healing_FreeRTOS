/**
 * Definition of the exception handlers.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __EXCEPTIONS_H_
#define __EXCEPTIONS_H_

// -- EXCEPTION HANDLERS ---
//secure world
void secure_undef();
void secure_swi(const unsigned int nIRQ);
void secure_pabort();
void secure_dabort();
void secure_irq();
void secure_fiq();

//normal world
void ns_reset();
void ns_undef();
void ns_swi(const unsigned int nIRQ);
void ns_pa();
void ns_da();
void ns_irq();
void ns_fiq();

#endif
