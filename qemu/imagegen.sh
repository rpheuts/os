#!/bin/bash

# Make a RAM disk of 1.44 MB as ExFAT
NUMSECTORS=51200
DEVNAME="disk.img"

# Create image
dd if=/dev/zero bs=1024 count=${NUMSECTORS} > ${DEVNAME}

# Partition image
fdisk -yia dos ${DEVNAME}

# Copy bootloader
fdisk -yu -f ../src/bootloader/boot.bin ${DEVNAME}

