GCC = gcc
CFLAGS = -march=i486 -m32 -nostdlib
APPFILES = ./*.c ./*.o

# TODO: いつか直す
hello.hrb: hello.asm Makefile
	nasm -f elf -o hello.hrb hello.asm

hello2.hrb: hello2.asm Makefile
	nasm -f elf -o hello2.hrb hello2.asm

HELLO3 = api.o hello3.c
hello3.hrb: hello3.c Makefile
	$(GCC) $(CFLAGS) -T ../har.lds $(HELLO3) -o hello3.hrb -fno-pic -Wl,-Map=hello3.map

HELLO4 = api.o hello4.c
hello4.hrb: hello4.c Makefile
	$(GCC) $(CFLAGS) -T ../har.lds $(HELLO4) -o hello4.hrb -fno-pic -Wl,-Map=hello4.map

hello5.hrb: hello5.asm Makefile
	nasm -f elf -o hello5.hrb hello5.asm

WINHELO = api.o winhelo.c
winhelo.hrb: winhelo.c
	$(GCC) $(CFLAGS) -T ../har.lds $(WINHELO) -o winhelo.hrb -fno-pic -Wl,-Map=winhelo.map

# なんとなく動くようになったら削除
BUG1 = bug1.c api.o
bug1.hrb: bug1.c api.o
	$(GCC) $(CFLAGS) -T ../har.lds $(BUG1) -o bug1.hrb -fno-pic -Wl,-Map=bug1.map

bug2.hrb: bug2.c
	$(GCC) $(CFLAGS) -T ../har.lds bug2.c -o bug2.hrb -fno-pic -Wl,-Map=bug2.map

BUG3 = bug3.c api.o
bug3.hrb: bug3.c
	$(GCC) $(CFLAGS) -T ../har.lds $(BUG3) -o bug3.o -fno-pic -Wl,-Map=bug3.map

api.o:
	nasm -f elf -o api.o api.asm

app.hrb: app.o
	$(GCC) $(CFLAGS) -T ../har.lds $(APPFILES) -o app.hrb -fno-pic -Wl,-Map=app.map
