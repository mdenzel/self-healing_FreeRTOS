/**
 *	Self-healing functionality
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef _SELFHEALING_H_
#define _SELFHEALING_H_

//helper function
int is_currentTCB_restoration();

//detection routine: checks pxCurrentTCB and maybe exchanges it with a restoration task
int detection();

//restoration routine: restores given task
void restoration(void* task);

#endif
