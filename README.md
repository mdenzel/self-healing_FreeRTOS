
# Self-healing FreeRTOS

```
@author:       Michael Denzel
@email:        research@michael-denzel.de
@license:      GNU General Public License 2.0 or later
@date:         2017-03-17
@version:      0.1
```

An ARM TrustZone port of FreeRTOS V9.0.0rc1 which can automatically self-heal
from certain attacks and runs on top of ARM TrustZone.

Source code includes code from:
- FreeRTOS V9.0.0rc1 -- www.freertos.org
- Dongli Zhang -- https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
- James Walmsley -- https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
- Francesco Balducci -- https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-program



## Quickstart

Just compile the tool using:

```
$ make
$ ./setupsd.sh /dev/sdX
```

`/dev/sdX` is the path to the SD-card which should be flashed with the image.

**Test system: Freescale i.MX53 Quick Start Board _ARM Cortex-A8_**

Required tools:
- arm-none-eabi-gcc
- arm-none-eabi-binutils
- arm-none-eabi-newlib



## Makefile Options

There are a couple of Makefile options:

#### Cleanup
Use `make clean` to clean the compiled files.

#### Logging and Printing
- `make info` - print some logging information
- `make smc` - print also the secure monitor calls (includes `info`)
- `make debug` - very verbose logging level (includes `info` and `smc`)

#### Timing analysis
- `make timingTz` - will compile a special image used for timing TrustZone
- `make timingNoTz` - will compile a special image used for timing the system without TrustZone

#### TrustZone
Normally the operating system will always be compiled for TrustZone. If that is
not desired, there is the option to compile using `make noTz`, even though we
recommend to compile with TrustZone.



## Attack and Self-healing

**Remark: by default the buffer overflow was turned off. To turn it on, one has to supply the compilation flag `BUFFEROVERFLOW`. See also file `./Demo/Drivers/IO.c`. Note that there is no Makefile option to supply this flag directly to prevent accidentially compiling a vulnerable image.**

TODO

![Attack](./attack.png "Attack")



