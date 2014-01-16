all: stage0/stage0.bin stage1/stage1.bin kernel/kernel.bin

clean:
	$(MAKE) -C ./stage0 clean
	$(MAKE) -C ./stage1 clean
	$(MAKE) -C ./kernel clean

stage0/stage0.bin:
	$(MAKE) -C ./stage0

stage1/stage1.bin:
	$(MAKE) -C ./stage1

kernel/kernel.bin:
	$(MAKE) -C ./kernel
