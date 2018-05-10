/**
 * Implemenation of a LED driver on the i.MX53 board and a simulated driver for
 * a temperature sensor.
 *
 * @author:       Michael Denzel
 * @email:        md.research@mailbox.org
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

//C
#include <stdlib.h> //for rand
#include <string.h>

// own includes
#include "IO.h"
#include "mxc_serial.h"


// --- LED driver ---
void set_led(LEDvalues led){
  //constants
  const unsigned int SW_MUX_BASE_ADDR = 0x53FA8000;
  const unsigned int IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1 = (SW_MUX_BASE_ADDR + 0x294);  
  const unsigned int GPIO7 = 0x53FE4000;
  const unsigned int gpio_base = GPIO7;
  const unsigned int bit = 7;

  //save IOMUX state
  const unsigned int iomux_state = (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1));
  
  //set ALT1 for GPIO7 => GPIO7 = GPIO7[7] = USR LED
  (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1)) = 0x1;
  
  //set to output mode
  (*(volatile unsigned int*)(gpio_base + 0x4)) = 1 << bit;
  unsigned int val = (*(volatile unsigned int*)(gpio_base+0x0));

  if(led == ON){
    //turn LED on
    (*(volatile unsigned int*)(gpio_base+0x0)) = val | (1 << bit);
#ifdef DEBUG
    cprintf("LED on\n");
#endif
  }else{
    //turn LED off
    (*(volatile unsigned int*)(gpio_base+0x0)) = val & ~(1 << bit);
#ifdef DEBUG
    cprintf("LED off\n");
#endif
  }
  
  //restore IOMUX state
  (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1)) = iomux_state;
}

LEDvalues get_led(){
  //constants
  const unsigned int SW_MUX_BASE_ADDR = 0x53FA8000;
  const unsigned int IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1 = (SW_MUX_BASE_ADDR + 0x294);  
  const unsigned int GPIO7 = 0x53FE4000;
  const unsigned int gpio_base = GPIO7;
  const unsigned int bit = 7;

  //save IOMUX state
  const unsigned int iomux_state = (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1));
  
  //set ALT1 for GPIO7 => GPIO7 = GPIO7[7] = USR LED
  (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1)) = 0x1;
  
  //set to output mode
  (*(volatile unsigned int*)(gpio_base + 0x4)) = 1 << bit;
  //get LED value
  unsigned int val = (*(volatile unsigned int*)(gpio_base+0x0)) & (1 << bit);

  //check LED status
  LEDvalues ret;
  if(val == 0){
    ret = ON;
  }else{
    ret = OFF;
  }
  
  //restore IOMUX state
  (*(volatile unsigned int*)(IOMUXC_SW_MUX_CTL_PAD_PATA_DA_1)) = iomux_state;

  //return LED status
  return ret;
}



// --- Thermal sensor ---
static temperature temp_config = {"init", 100, 0};
static int temp = 50;

void reset_temperature(){
  strcpy(temp_config.info, "init");
  temp_config.max = 100;
  temp_config.min = 0;
}

int check_temperature(){
  //IMPROVE: "read" temperature from sensor
  int r = ((rand() % 4) - 1) * 5; //[-5; 0; +5; +10]
  temp += r;
  
  if(temp < temp_config.min){
    //IMPROVE: "update" actuator
    cprintf("heating...\n");
    temp += 10;
  }
  else if(temp > temp_config.max){
    //IMPROVE: "update" actuator
    cprintf("cooling...\n");
    temp -= 10;
  }
  
  return temp;
}

temperature get_config_temperature(){
  return temp_config;
}

void set_config_temperature(char* str){
#ifdef BUFFEROVERFLOW
  strcpy(temp_config.info, str); //buffer overflow (on purpose)!!!
#else
  strncpy(temp_config.info, str, 15);
#endif
}
