/**
 * A few tasks to demonstrate self-healing FreeRTOS.
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

//lib
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h> //for atoi
#include <string.h> //strcpy

//own includes
#include "controltasks.h"
#include "mxc_serial.h"
#include "IO.h"
#include "interrupts.h"
#include "trustzone.h" //for print_WORLD and print_MODE
#include "timing.h"

// --- TASKS ---
char* const          TASK_NAMES[NUM_TASKS]     = {"task1",              "task2",     "task3"};
const unsigned int   TASK_PRIOS[NUM_TASKS]     = {configMAX_PRIORITIES, 1,           2};
void (*TASK_FUNCTIONS[NUM_TASKS])(void*)       = {controltask,          controltask, controltask};


// --- UTILITY ---
void ListTasks(){
  //roughly 40bytes per task => 64 bytes per task should be sufficient
  //(plus IDLE plus restoration)
  char* tasklist = malloc(64*uxTaskGetNumberOfTasks());
  vTaskList(tasklist);
  cprintf("\ntasklist:\n");
  cprintf("task\t\tstate\tprio\tstack\ttasknum\n");
  cprintf("---------------------------------------------------\n");
  cprintf(tasklist);
  cprintf("('B'locked, 'R'eady, 'D'eleted, 'S'uspended)\n");
  cprintf("\n");
  free(tasklist);
  tasklist = NULL;
}

// --- OWN TASK IMPLEMENTATION ---
void controltask(void *pParam){
  cprintf("%s start\n", (char*)pParam);
  
  //init
  const unsigned int num = atoi( ((char*)pParam)+4 );

  //local variables
  const portTickType delay = num /*seconds*/ * portTICK_PERIOD_MS; 
  
  //run
  while(1) {
    //functionality
#ifdef INFO
    cprintf("%s IRQ: %d, FIQ: %d\n", (char*)pParam, CheckIRQ(), CheckFIQ());
    print_MODE();
    print_WORLD();
#endif
    
    switch(num){
    case 1:
      //task 1: toggle LED every second
      set_led(OFF);
      vTaskDelay(delay*2);
      set_led(ON);
      vTaskDelay(delay*2);
      break;
    case 2:
      //task 2: first attacks
      /*
      //timer IRQ
      cprintf("trying to register IRQ 40\n");
      cprintf("result: %d\n", RegisterInterrupt(40, NULL, NULL));

      //undefined SMC
      cprintf("undefined SMC\n");
      __asm volatile("SMC #14");

      //reset interrupt controller
      cprintf("reset interrupt controller\n");
      InitInterruptController();

      //set temp
      cprintf("overheating...\n");
      char attack[21]; //overflow by 5 bytes (=int for max temp + '\0')
      strcpy(attack, "1234567890123456"); //fill buffer
      *((int*)(attack+16)) = 150; //overflow => set max temp integer
      attack[20] = '\0'; //null-termination (will affect min temp if min temp != 0)
      set_config_temperature(attack);
      cprintf("max: %d\n", get_config_temperature().max);
      
      //DoS
      //FIXME: DoS actually works against FreeRTOS: the idle task is suppose to clean up the tasklist and free memory
      //FIXME: it is possible that tasks starve with the FreeRTOS scheduler! (it apparently relies on tasks to be somewhat cooperative => they have to call vTaskDelay at some point if they have a high priority)
      //cprintf("DoS\n");
      //while(1);
      */

      /*
      ;
      init_timer();
      const unsigned int s = 88; //sizeof(struct tskTaskControlBlock) = 88
      for(unsigned int i = 0; i < 1530; ++i){
        start_timer();
        void* p = malloc(s);
        unsigned int t = stop_timer();
        cprintf("malloc %u: %u ns\n", i, t);
        free(p);
      }
      */
      
      cprintf("temperature logging: %d\n", check_temperature());
      vTaskDelay(delay * 2);

      
      break;
    case 3:
      //task 3: read user input
    {
      //variables
      const unsigned int size = 128;
      char buf[size];
      buf[0] = '\0';
#ifdef INFO
      cprintf("%s reading user input\n", (char*)pParam);
#endif
      //blocking read
      if(readline(buf, size) != 0){
        //user input in colour
        cprintf("\e[1;32muser input: '%s'\e[0m\n", buf);

        //handle user input
        if(strcmp(buf, "status") == 0){
          //print status
          temperature conf = get_config_temperature();
          int t = check_temperature();
          cprintf("config: '%s'\n[%d; %d]\ntemp: %d\n", conf.info, conf.min, conf.max, t);
        }
        else if(strncmp(buf, "update ", 7) == 0){
          //update sensor.info (incl. a buffer overflow depending on compilation)
          cprintf("updating\n");
          set_config_temperature(buf+7);
          temperature conf = get_config_temperature();
          cprintf("config: '%s'\n[%d; %d]\n", conf.info, conf.min, conf.max);
        }
        //else: ignore
      }
      //sleep
      vTaskDelay(delay);
    }
    break;
    
    default:
#ifdef INFO
      cprintf("%s going to sleep\n", (char*)pParam);
#endif
      //sleep
      vTaskDelay(10 * portTICK_PERIOD_MS);
    }
  }

  //should never happen, just for safety
  vTaskDelete(NULL);
}
