obj-m := jitqueue.o
	
KDIR := /usr/src/linux-2.6.38
all:
	make -C $(KDIR) M=$(PWD) 
clean:
	rm -f *.o *.mod.o *.mod.c *.symvers *.ko *.order
