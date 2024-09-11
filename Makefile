obj-m += firewall.o  # Add the kernel module object

all:
	# Compile the kernel module
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

api_program: iran_block_api.c
	# Compile the user-space API program
	gcc -o iran_block_api iran_block_api.c

clean:
	# Clean the build files
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f iran_block_api