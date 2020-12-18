#!/bin/bash

qemu-system-x86_64 \
	-cpu  Nehalem -smp 2 \
	-m 512 \
	-bios /usr/share/ovmf/OVMF.fd \
	-hda fat:rw:./test \
    -soundhw pcspk
    -net none
	#-net tap,vlan=0 -net nic,vlan=0,model=e1000,macaddr=52:54:00:12:34:56
	#-net tap,ifname=tap0,script=no,downscript=no
