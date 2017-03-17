#!/bin/bash

#
# An example attack to demonstrate self-healing functionality of adjusted FreeRTOS.
#
# @author:       Michael Denzel
# @email:        research@michael-denzel.de
# @license:      GNU General Public License 2.0 or later
# @date:         2017-03-17
# @version:      0.1
#

### CONFIG ###
io=/dev/ttyUSB0
##############

if [ $# -eq 1 ]; then
    hex=`echo "obase=16; $1" | bc`
    echo -e "update 1234561234567890\x$hex" > $io
    echo -e "status" > $io
else
    echo "Usage: $0 <number>"
fi
