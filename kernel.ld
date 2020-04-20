/**
 * Linker Script for the Cortex-A8 (location 0x97800000)
 *
 * @author:       Michael Denzel
 * @contact:      https://github.com/mdenzel
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

ENTRY(_start)

SECTIONS
{
	. = 0x97800000;
	/*. = ALIGN(4);*/
	.text : { *(.text) }

	. = ALIGN(4);
	.data : { *(.data) }

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss*) }
	__bss_end = .;

    /*
    * heap trick (XXX: see also Demo/main.c)
    * source: Francesco Balducci https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/
    */
    . = ALIGN(8);
    heap_low = .;    /* for _sbrk */
    . = . + 0x10000; /* 64kB of heap memory */
    heap_top = .;    /* for _sbrk */

	/*
	 * stack
	 */
	. = 0xa0000000;
	.stack :
	{
		STACK_ADDR = . ;
	}
}

