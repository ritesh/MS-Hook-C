CC=llvm-gcc
CFLAGS=-Wall

all: 
	$(CC) MethodScanner.c -o methodscanner
	$(CC) test.c -o test
clean:
	rm -f *.out
	rm -f test methodscanner


	
