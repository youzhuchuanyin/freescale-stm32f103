obj-m +=led_drv.o 
obj-m +=led_dev.o
#obj-m +=btn_dev.o
#obj-m +=btn_drv.o
all:
	make -C /opt/kernel SUBDIRS=$(PWD) modules
	#arm-cortex_a9-linux-gnueabi-gcc -o btntest btn_test.c
	cp *.ko /opt/rootfs/home/drivers/
	#cp *.ko  /opt/rootfs/home/drivers/

clean:
	make -C /opt/kernel SUBDIRS=$(PWD) clean
