#!/bin/sh
make
insmod 4mbdevice.ko
mknod /dev/testdevice c 66 0
