# Kernel module name
obj-m += firewall.o

# Directory containing the kernel build system
KDIR := /lib/modules/$(shell uname -r)/build

# User-space executable name
USER_EXEC = firewall_api

# Default rule to build the kernel module and the user-space API
all: kernel user

# Rule to build the kernel module
kernel:
	make -C $(KDIR) M=$(PWD) modules

# Rule to build the user-space API
user:
	gcc firewall_api.c -o $(USER_EXEC)

# Clean the build directory
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(USER_EXEC)