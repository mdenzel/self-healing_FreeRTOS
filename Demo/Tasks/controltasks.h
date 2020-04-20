/**
 * A few tasks to demonstrate self-healing FreeRTOS.
 *
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __CONTROLTASKS_H_
#define __CONTROLTASKS_H_

// --- UTILITY ---
void ListTasks();

// --- OWN TASK IMPLEMENTATION ---
//task management
#ifdef TIMING
#define NUM_TASKS 4
#else
#define NUM_TASKS 3
#endif
extern char* const TASK_NAMES[];
extern const unsigned int TASK_PRIOS[];
extern void (*TASK_FUNCTIONS[])(void*);

//functions
void controltask(void *pParam);

#endif
