/**
 * Definitions for a timing analysis on FreeRTOS (TrustZone) task swtich.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __TIMING_H_
#define __TIMING_H_

//Module with a simple timer

#define TIME_AVERAGE 60 //average print every X seconds

//keep module small to not affect performance too much
void init_timer();
void start_timer();
unsigned int stop_timer();
unsigned int timer_average();
unsigned int timer_num();
unsigned int get_timer_overhead();

#endif 
