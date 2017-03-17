/**
 * Printing functionality (via serial cable).
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 *
 * XXX: file is based on code from
 *      Dongli Zhang https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
 **/

#ifndef __MXC_SERIAL_H__
#define __MXC_SERIAL_H__

//init
void iomuxc_init(void);
void mxc_serial_init(void);

//read/write
void cprintf(char *fmt, ...);
unsigned int readline(char* const buf, const unsigned int size);

#endif
