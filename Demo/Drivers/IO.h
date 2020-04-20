/**
 * Definitions for a LED driver on the i.MX53 board and a simulated driver for a
 * temperature sensor.
 *
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef __IO_H_
#define __IO_H_

// --- LED driver ---
typedef enum LEDvalues {
  ON,
  OFF
} LEDvalues;

void set_led(LEDvalues val);
LEDvalues get_led();

// --- Thermal sensor ---
typedef struct temperature{
  char info[16]; //to introduce a buffer overflow
  int max;
  int min;
} temperature;
void reset_temperature();
int check_temperature();
temperature get_config_temperature();
void set_config_temperature(char* str); //to introduce a buffer overflow

#endif

