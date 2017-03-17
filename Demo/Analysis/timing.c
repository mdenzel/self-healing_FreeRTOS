/**
 * Timing analysis on FreeRTOS (TrustZone) task swtich.
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

//own includes
#include "timing.h"
//FreeRTOS includes
#include "FreeRTOSConfig.h"

// --- Variables ---
//timing
static unsigned int timing_overhead = 0;
static unsigned int timing = 0;
//incremental average
static unsigned int num_timings = 0;
static unsigned int average = 0;

// --- FUNCTIONS ---

void init_timer(){
  //enable user-mode access
  __asm volatile("MCR p15, 0, %0, c9, c14, 0\n\t" :: "r"(1));
  //disable overflow interrupt
  __asm volatile("MCR p15, 0, %0, c9, c14, 2\n\t" :: "r"(0x8000000f));

  unsigned int value = 1; //enable all counters
  value |= 2; //reset all counters to zero
  value |= 4; //reset cycle counter to zero
  //value |= 8; // enable "by 64" divider for CCNT (for long times)
  value |= 16;
  __asm volatile("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

  //enable all counters
  __asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));
  //clear overflows
  __asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));

  //overhead
  start_timer();
  timing_overhead = stop_timer();
}

void start_timer(){
  //FreeRTOS implmentation (xTaskGetTickCount) only works after scheduler was started
  //read CCNT (cycle count) Register => CPU 1GHz => nano seconds
  __asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(timing));
}

unsigned int stop_timer(){
  //read CCNT Register
  unsigned int val = 0;
  __asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val));
  val = (val - timing - timing_overhead);
  //update average
  ++num_timings;
  average = ((num_timings-1) * average + val) / num_timings;
  return val;
}

unsigned int timer_average(){
  return average;
}

unsigned int timer_num(){
  return num_timings;
}

unsigned int get_timer_overhead(){
  return timing_overhead;
}

