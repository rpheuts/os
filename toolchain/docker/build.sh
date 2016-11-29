#!/bin/bash

make clean
make

# Make a disk-image of 50 MB
NUMSECTORS=51200
DEVNAME="disk.img"

# Create image
dd if=/dev/zero bs=1024 count=${NUMSECTORS} > ${DEVNAME}

# Provision with exFat
mkfs -t vfat ${DEVNAME}

# Copy stage0 bootloader
dd if=/usr/src/stage0/stage0.bin of=${DEVNAME} bs=446 conv=notrunc

# Copy stage1
dd if=/usr/src/stage1/stage1.bin of=${DEVNAME} seek=1 bs=512 conv=notrunc
dd if=/usr/src/kernel/kernel.bin of=${DEVNAME} seek=3 bs=512 conv=notrunc