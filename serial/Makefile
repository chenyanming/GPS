testarm: test.c serial.c gps.c 
	arm-linux-gcc -g test.c serial.c gps.c  -o testarm
debugarm: test
	arm-linux-gdb test
testx86: test.c serial.c gps.c 
	gcc -g test.c serial.c gps.c  -o testx86
debugx86: test
	cgdb test
clean:
	rm testarm testx86
