ifneq ($(KERNELRELEASE),)

# We are called from the kernel build system
scull-objs := main.o
obj-m := scull.o

else

KERN_SRC := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	make -C $(KERN_SRC) M=$(PWD) modules

endif

clean:
	make -C $(KERN_SRC) M=$(PWD) clean
