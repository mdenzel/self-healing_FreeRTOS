#!/bin/bash

#
# Shellscript to flash a compiled kernel image (kernel.img) to an sd-card.
# (/dev/sda is rejected as input as this is usually the own harddrive - adjust if needed)
#
# @author:       Michael Denzel
# @contact:      https://github.com/mdenzel
# @license:      GNU General Public License 2.0 or later
# @date:         2017-03-17
# @version:      0.1
#


if [ $# -ne 1 ]; then
    echo "setupsd.sh <sd-card>"
    exit -1
fi

if [ $1 == "/dev/sda" ]; then
    echo "reject $1"
    exit -1
fi

#create image
mkimage -n imximage.cfg -T imximage -e 0x97800000 -d kernel.img kernel.imx

#flash it to SD card
dd if=kernel.imx of=$1 bs=512 seek=2

sync
