OS
==

This is a hobby and study project and has no direct purpose (besides having some fun). The intent is to write an Operating System from scratch, including the bootloader and experimenting with new ways of solving old problems and bottlenecks in current Operating Systems. While the results in the end might not reflect the intententions that were set out at the beginning, it is never meant to be anything more than hobby.

toolchain
---------

The script will compile and configure a cross-compiler (GCC) on an OSX machine (and probably on a Unix machine too). As the actual OS is aspiring to be native 64-bit, this toolchain will be expanded to include a 64-bit cross compiler in the future.

qemu
----

The script(s) in there will help in prepping a disk image for running with QEMU. The tools will evolve as progress is made...

src
---

The sources for the bootloader and OS
