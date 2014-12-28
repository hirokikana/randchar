obj-m := randchar.o
clean-files := *.o *.ko *.mod.[co] *~

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
