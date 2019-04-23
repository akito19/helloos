GCC = gcc
CFLAGS = -march=i486 -m32 -nostdlib
APPFILES = ./*.c ./*.o
APP_NAME = a

# TODO: いつか直す
hello.hrb: hello.asm Makefile
	nasm -f elf -o hello.hrb hello.asm

hello2.hrb: hello2.asm Makefile
	nasm -f elf -o hello2.hrb hello2.asm

HELLO3 = a.o hello3.c
hello3.hrb: hello3.c Makefile
	$(GCC) $(CFLAGS) -T ../har.lds $(HELLO3) -o hello3.hrb -fno-pic -Wl,-Map=hello3.map

crack1.hrb: crack1.c
	$(GCC) $(CFLAGS) -T ../har.lds crack1.c -o crack1.hrb -fno-pic -Wl,-Map=crack1.map

crack2.hrb: crack2.asm
	nasm -f elf -o crack2.hrb crack2.asm

app.o:
	nasm -f elf -o $(APP_NAME).o $(APP_NAME).asm

app.hrb: app.o
	$(GCC) $(CFLAGS) -T ../har.lds $(APPFILES) -o app.hrb -fno-pic -Wl,-Map=app.map
