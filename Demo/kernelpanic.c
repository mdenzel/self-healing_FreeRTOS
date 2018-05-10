/**
 * Kernel panic functionality if something goes horribly wrong.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#include "kernelpanic.h"
#include "mxc_serial.h"
#include "interrupts.h"

void kernelpanic(const char* const string){
  //print error message (with additional newline)
  cprintf("%s", string);
  cprintf("\n");

  //stop everything
  __asm volatile("SMC #13\n");
}
