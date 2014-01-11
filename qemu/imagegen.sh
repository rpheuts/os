#!/bin/bash

# Make a disk-image of 1.44 MB
NUMSECTORS=51200
DEVNAME="disk.img"

# Create image
dd if=/dev/zero bs=1024 count=${NUMSECTORS} > ${DEVNAME}

# Provision with exFat
fdisk -yia dos ${DEVNAME}

# Copy stage0 bootloader
fdisk -yu -f ../src/stage0/stage0.bin ${DEVNAME}

# Copy stage1
dd if=../src/stage1/entry.bin of=${DEVNAME} seek=1 bs=512 conv=notrunc
dd if=../src/stage1/main.bin of=${DEVNAME} seek=2 bs=512 conv=notrunc
