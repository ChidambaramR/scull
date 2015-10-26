CONFIG_MODULE_SIG=n

DEBUG=y

ifeq ($(DEBUG),y)
	DEBFLAGS = -o0 -g -DSCULL_DEBUG
endif
# EXTRA_CFLAGS apply onlt to kernel (kbuild) make file.
EXTRA_CFLAGS += $(DEBFLAGS)

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
