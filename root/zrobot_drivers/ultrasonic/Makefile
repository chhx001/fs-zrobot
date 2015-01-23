ifneq ($(KERNELRELEASE),)
obj-m := ultrasonic_driver.o
else
KERNEL_DIR := /home/xup/cross/linux-zybo/linux-digilent
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules ARCH=arm
clean:
	rm *.o *.ko *.mod.c Module* module*
endif
