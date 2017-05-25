/**
 * TrustZone functionality to self-heal failed or misbehaving tasks.
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

//FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
//C
#include <string.h>
#include <stdlib.h>
#include <stdio.h> //sprintf
//own includes
#include "selfhealing.h"
#include "mxc_serial.h"
#include "trustzone.h"
#include "kernelpanic.h"
#include "controltasks.h"
#include "interrupts.h"
//Drivers
#include "IO.h"

extern volatile void* volatile pxCurrentTCB;

//types
typedef struct tskTaskControlBlock
{
  StackType_t* topOfStack;
  ListItem_t   unused1;
  ListItem_t   unused2;
  UBaseType_t  prio;
  StackType_t* stack;
  char         name[ configMAX_TASK_NAME_LEN ];
} MiniTCB_t;


// --- HELPERS ---

//checks if the currentTCB is the restoration task
//IMPROVE: normally this should not be done via the name but via it's PC (program counter)
int is_currentTCB_restoration(){
  return strcmp(((MiniTCB_t*)pxCurrentTCB)->name, "restoration");
}

//replaces actual (malicious) task with the self-healing routine
static void schedule_restoration(void* task){
  cprintf("\e[0;33m"); //yellow colour
#ifdef DEBUG
  //IRQ/FIQ should be off!
  cprintf("schedule_restoration IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif

#ifdef INFO
  cprintf("schedule_restoration: create restoration task\n");
#endif
  //create restoration task at priority of the misbehaving task
  //xTaskCreate(function, name, stacksize, parameter, priority, handle);
  TaskHandle_t restoration_task = NULL;
  if(xTaskCreate(restoration, "restoration", 256, task,
                 ((MiniTCB_t*)task)->prio, &restoration_task) != pdPASS){
    kernelpanic("could not create restoration task\n");
  }
  if(restoration_task == NULL){
    kernelpanic("restoration task NULL\n");
  }

  //set restoration next instead of malicious task
  pxCurrentTCB = (void*)restoration_task;
#ifdef INFO
  cprintf("new task: %s\n", ((MiniTCB_t*)pxCurrentTCB)->name);
#endif

#ifdef INFO
  cprintf("schedule_restoration: suspend %s\n", ((MiniTCB_t*)task)->name);
#endif
  //suspend current misbehaving task (will trigger rescheduling)
  //since the task is suspended, detection will not run for it and
  //there won't be 2 restoration routines for one task
  vTaskSuspend(task);
  cprintf("\e[0m"); //default colour
}


#ifdef DEBUG
//debug function to print a `num` values on the stack
__attribute__((unused)) static void print_stack(const volatile StackType_t* stack, const unsigned int num){
  for(int i = num-1; i >= 0; --i){
    cprintf("stack %x: %x\n", stack+i, *(stack+i));
  }
}
#endif

// =================
// --- DETECTION ---
// =================

int detection(){
  //variables
  MiniTCB_t* tcb = NULL;
  int ret = 0;
  
  //prevent scheduler from swapping out the detection/restoration_scheduling routine
  vTaskSuspendAll();
  {
#ifdef DEBUG
    //IRQ/FIQ should be off!
    cprintf("detection IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif
    
    //get current task control block
    tcb = (MiniTCB_t*)pxCurrentTCB;

    //log
#ifdef INFO
    cprintf("-- detection for %s --\n", tcb->name);
#endif
#ifdef DEBUG
    //debug prints
    print_MODE();
    print_WORLD();
#endif

    //IMPROVE: detection: sensor values via SMC?
    //task3
    if(strcmp(tcb->name, "task3") == 0){
      temperature t = get_config_temperature();
#ifdef INFO
      cprintf("temp: (%s, %d, %d)\n", t.info, t.max, t.min);
#endif
      //IMPROVE: "policy" for now hard-coded
      if(t.max > 100){
        schedule_restoration(tcb);
        ret = -13;
      }
    }
  }//finished => resume scheduler
  xTaskResumeAll();

#ifdef DEBUG
  //IRQ/FIQ should be off!
  cprintf("detection IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif
#ifdef INFO
  cprintf("detection new task: %s\n", ((MiniTCB_t*)pxCurrentTCB)->name);
  cprintf("-- detection for %s done --\n", tcb->name);
#endif
#if defined(DEBUG) || defined(INFO)
  cprintf("\n");
#endif

  return ret;
}


// ===================
// --- RESTORATION ---
// ===================

void restoration(void* malicious_task){
  //restoration: for now, remove task and hang it in again
#ifdef INFO
  cprintf("\e[0;31m-- restoration for %s --\e[0m\n", ((MiniTCB_t*)malicious_task)->name);
#endif
#ifdef DEBUG
  //debug prints
  cprintf("\e[0;31m");
  print_MODE();
  print_WORLD();
  cprintf("\e[0m");
  cprintf("\e[0;31mrestoration: IRQ: %d, FIQ: %d\e[0m\n", CheckIRQ(), CheckFIQ());
#endif
  
  //get number of task
  const unsigned int tasknum = atoi((((MiniTCB_t*)malicious_task)->name)+4);
  //remove task
#ifdef INFO
  cprintf("\e[0;31mrestoration: removing %s\e[0m\n", ((MiniTCB_t*)malicious_task)->name);
#endif
  vTaskDelete(malicious_task);
  //FIXME: something enables IRQ/FIQ before here (vTaskDelete!)
#ifdef DEBUG
  cprintf("restoration: IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif
  
  //restore sensor/actuators/data
#ifdef INFO
  cprintf("\e[0;31mrestoration: restoring temp sensor\e[0m\n");
#endif
  //IMPROVE: restoration for now hard-coded (should be pulled from server)
  reset_temperature();

  //recreate task
#ifdef INFO
  cprintf("\e[0;31mrestoration: recreating task num %d\e[0m\n", tasknum);
#endif

  DisableInterrupts();
  vTaskSuspendAll();
  {
#ifdef DEBUG
    //IRQ/FIQ should be off!
    cprintf("restoration xTaskCreate: IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());
#endif
    if(xTaskCreate(TASK_FUNCTIONS[tasknum-1], TASK_NAMES[tasknum-1], 256,
                   TASK_NAMES[tasknum-1], TASK_PRIOS[tasknum-1], NULL) != pdPASS){
      char* err = malloc(32);
      sprintf(err, "could not re-create task %u\n", tasknum);
      kernelpanic(err);
    }
#ifdef INFO
  cprintf("\e[0;31m-- restoration for %s done --\e[0m\n", ((MiniTCB_t*)malicious_task)->name);
#endif
  }//finished => resume scheduler
  xTaskResumeAll();
  EnableInterrupts();
  
  //remove this restoration task
  vTaskDelete(NULL);
}
