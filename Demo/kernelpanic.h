/**
 * Kernel panic functionality if something goes horribly wrong.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __KERNEL_PANIC_
#define __KERNEL_PANIC_

void kernelpanic(const char* const message);

#endif
