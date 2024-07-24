obj-m += reverse.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	-sudo rmmod reverse.ko
	sudo dmesg -C
	sudo insmod reverse.ko
	dmesg
