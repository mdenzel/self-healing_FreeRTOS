
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

TODO: coming shortly, for now see Makefile and setupsd.sh

Required tools:
- arm-none-eabi-gcc
- arm-none-eabi-binutils
- arm-none-eabi-newlib

