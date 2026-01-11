obj-m +=hcsr04_drv.o 
obj-m +=hcsr04_dev.o
#obj-m +=hcsr_dev.o
#obj-m +=hcsr_drv.o
all:
	make -C /opt/kernel SUBDIRS=$(PWD) modules
	#arm-cortex_a9-linux-gnueabi-gcc -o btntest btn_test.c
	cp *.ko /opt/rootfs/home/drivers/
	#cp *.ko  /opt/rootfs/home/drivers/

clean:
	make -C /opt/kernel SUBDIRS=$(PWD) clean
