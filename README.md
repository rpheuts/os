OS
==

This is a hobby and study project and has no direct purpose (besides having some fun). The intent is to write an Operating System from scratch, including the bootloader and experimenting with new ways of solving old problems and bottlenecks in current Operating Systems. While the results in the end might not reflect the intententions that were set out at the beginning, it is never meant to be anything more than hobby.

toolchain
---------

The script in 'local' will compile and configure a cross-compiler (GCC) on an OSX machine (and probably on a Linux machine too).

In the 'vagrant' you'll find an 'environment-in-a-box' using Vagrant and Chef. Just run 'vagrant up' and it should build a custom VM for you that contains the required assembler and crosscompilers (32 & 64-bit) plus a mapped directory into the VM with the sources. You should be able to build the sources from within the VM (vagrant ssh) and then run it on the host machine with QEMU. 

qemu
----

The script(s) in there will help in prepping a disk image for running with QEMU. The tools will evolve as progress is made...

src
---

The sources for the bootloader stages and kernel / OS
