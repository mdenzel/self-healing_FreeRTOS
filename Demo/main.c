/**
 * Main file loading TrustZone, starting FreeRTOS, and running an example of
 * self-healing tasks (for i.MX53 board).
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 *
 * XXX: file includes code from
 *      Dongli Zhang https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
 *      and
 *      James Walmsley https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
 *      and
 *      Francesco Balducci https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/
 **/

//lib
#include <FreeRTOS.h>
#include <task.h>
#include <sys/stat.h> //for _sbrk
#include <stdlib.h> //for malloc
#include <stdio.h> //for sprintf

//own includes
//Drivers
#include "mxc_serial.h"
#include "kernelpanic.h"
#include "IO.h"
//TrustZone
#include "interrupts.h"
#include "trustzone.h"
#include "selfhealing.h"
//Tasks
#include "controltasks.h"

//Timing
#ifdef TIMING
#include "timing.h"
#endif

//globals
#define R32   (volatile unsigned long *)
#define CSU_BASE_ADDR   0x63F9C000

//external variables
extern TaskHandle_t restoration_task;

//external functions
extern void InitTrustzone();
extern void InitSecureWorldWithoutTrustZone();

enum{
  CSL0	= CSU_BASE_ADDR + 0x00,
  CSL1	= CSU_BASE_ADDR + 0x04,
  CSL2	= CSU_BASE_ADDR + 0x08,         // GPIO7
  CSL3	= CSU_BASE_ADDR + 0x0c,
  CSL4	= CSU_BASE_ADDR + 0x10,
  CSL5	= CSU_BASE_ADDR + 0x14,
  CSL6	= CSU_BASE_ADDR + 0x18,
  CSL7	= CSU_BASE_ADDR + 0x1c,
  CSL8	= CSU_BASE_ADDR + 0x20,
  CSL9	= CSU_BASE_ADDR + 0x24,
  CSL10	= CSU_BASE_ADDR + 0x28,
  CSL11 = CSU_BASE_ADDR + 0x2c,
  CSL12	= CSU_BASE_ADDR + 0x30,         // GPIO7
  CSL13	= CSU_BASE_ADDR + 0x34,
  CSL14	= CSU_BASE_ADDR + 0x38,
  CSL15	= CSU_BASE_ADDR + 0x3c,
  CSL16	= CSU_BASE_ADDR + 0x40,
  CSL17	= CSU_BASE_ADDR + 0x44,
  CSL18	= CSU_BASE_ADDR + 0x48,
  CSL19 = CSU_BASE_ADDR + 0x4c,
  CSL20	= CSU_BASE_ADDR + 0x50,
  CSL21	= CSU_BASE_ADDR + 0x54,
  CSL22	= CSU_BASE_ADDR + 0x58,
  CSL23	= CSU_BASE_ADDR + 0x5c,
  CSL24	= CSU_BASE_ADDR + 0x60,
  CSL25	= CSU_BASE_ADDR + 0x64,         // ESDHC
  CSL26	= CSU_BASE_ADDR + 0x68,
  CSL27	= CSU_BASE_ADDR + 0x6c,
  CSL28	= CSU_BASE_ADDR + 0x70,         // ESDHC
  CSL29	= CSU_BASE_ADDR + 0x74,
  CSL30	= CSU_BASE_ADDR + 0x78,
  CSL31	= CSU_BASE_ADDR + 0x7c,
};

void enable_hwfirewall(void){
  *R32 CSL0  = 0x00ff00ff;
  *R32 CSL1  = 0x00ff00ff;
  *R32 CSL2  = 0x00ff00ff;
  *R32 CSL3  = 0x00ff00ff;
  *R32 CSL4  = 0x00ff00ff;
  *R32 CSL5  = 0x00ff00ff;
  *R32 CSL6  = 0x00ff00ff;
  *R32 CSL7  = 0x00ff00ff;
  *R32 CSL8  = 0x00ff00ff;
  *R32 CSL9  = 0x00ff00ff;
  *R32 CSL10 = 0x00ff00ff;
  *R32 CSL11 = 0x00ff00ff;
  *R32 CSL12 = 0x00ff00ff;
  *R32 CSL13 = 0x00ff00ff;
  *R32 CSL14 = 0x00ff00ff;
  *R32 CSL15 = 0x00ff00ff;
  *R32 CSL16 = 0x00ff00ff;
  *R32 CSL17 = 0x00ff00ff;
  *R32 CSL18 = 0x00ff00ff;
  *R32 CSL19 = 0x00ff00ff;
  *R32 CSL20 = 0x00ff00ff;
  *R32 CSL21 = 0x00ff00ff;
  *R32 CSL22 = 0x00ff00ff;
  *R32 CSL23 = 0x00ff00ff;
  *R32 CSL24 = 0x00ff00ff;
  *R32 CSL25 = 0x003b003b;
  *R32 CSL26 = 0x00ff00ff;
  *R32 CSL27 = 0x00ff00ff;
  *R32 CSL28 = 0x003b003b;
  *R32 CSL29 = 0x00ff00ff;
  *R32 CSL30 = 0x00ff00ff;
  *R32 CSL31 = 0x00ff00ff;
}


//XXX: heap trick from https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/
char *heap_end = 0;
caddr_t _sbrk(int incr) {
  extern char heap_low; //defined by the linker
  extern char heap_top; //defined by the linker
  char *prev_heap_end;

  if (heap_end == 0) {
    heap_end = &heap_low;
  }
  prev_heap_end = heap_end;

  if (heap_end + incr > &heap_top) {
    //heap and stack collision
    return (caddr_t)0;
  }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}


/**
 *  Main entry point for system
 **/
void bootmain(void){
  // --- INIT ---
#ifdef TIMING
  //setup and start timer
  init_timer();
  start_timer();
#endif
  //enable_hwfirewall allows printf in Normal World
  enable_hwfirewall();
  //setup serial console
  iomuxc_init();
  mxc_serial_init();
  set_led(OFF);
#ifdef TRUSTZONE
  //setup trustzone
  cprintf("init TrustZone\n");
  InitTrustzone();
#else
  //setup secure world
  cprintf("init secure world without TrustZone\n");
  InitSecureWorldWithoutTrustZone();
#endif
  //setup interrupts
  cprintf("init TZIC\n");
  InitInterruptController(); //TrustZone Interrupt Controller TZIC
  ClearAllInterrupts();
#ifdef TIMING
  cprintf("startup time: %d ns\n", stop_timer());
  cprintf("timing overhead: %d ns\n", get_timer_overhead());
#endif

  
  cprintf("\n============================\n=========== MAIN ===========\n============================\n");
  print_MODE();
#ifdef TRUSTZONE
  //check trustzone
  if(SMC_is_secure_world() == 1){
    cprintf("secure world\n");
  }else{
    kernelpanic("normal world even though trustzone enabled\n");
  }
#else
  cprintf("trustzone disabled\n");
#endif
  //print interrupts
  cprintf("IRQ: %d, FIQ: %d\n", CheckIRQ(), CheckFIQ());

  // --- FreeRTOS ---
  cprintf("\n--- FreeRTOS (%s) ---\n", tskKERNEL_VERSION_NUMBER);
  //create tasks
  //idle task is automatically created by scheduler  
  for(unsigned int i = 0; i < NUM_TASKS; ++i){
    //xTaskCreate(function, name, stacksize, parameter, priority, handle);
    if(xTaskCreate(TASK_FUNCTIONS[i], TASK_NAMES[i], 256,
                   TASK_NAMES[i], TASK_PRIOS[i], NULL) != pdPASS){
      char* err = malloc(32);
      sprintf(err, "could not create task %u\n", i);
      kernelpanic(err);
    }
  }
  
  //list tasks
  ListTasks();
  
  //start scheduling
  cprintf("scheduler\n");
  vTaskStartScheduler();
  
  //we should never return from the main!
  kernelpanic("main returned\n");
}
