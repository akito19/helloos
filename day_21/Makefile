OSNAME = haribote
GCC = gcc
CFLAGS = -march=i486 -g -m32 -nostdlib
APPS := ./app/*.hrb
UTIL := ../utils

default:
	make img

ipl10.bin: ipl10.asm Makefile
	nasm ipl10.asm -o ipl10.bin -l ipl10.lst

asmhead.bin: asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.bin -l asmhead.lst

nasmfunc.o: nasmfunc.asm Makefile
	# -g ; generate debugging information
	# -f : select an output format
	nasm -g -f elf -o nasmfunc.o nasmfunc.asm

hankaku.c: $(UTIL)/hankaku.c Makefile
	cp $(UTIL)/hankaku.c ./

FILES = bootpack.c hankaku.c graphic.c dsctbl.c int.c \
				fifo.c keyboard.c mouse.c memory.c sheet.c \
				timer.c mtask.c window.c console.c file.c \
				mysprintf.c strncmp.c nasmfunc.o
bootpack.hrb: $(FILES) har.lds Makefile
	# http://softwaretechnique.jp/OS_Development/startup_kernel.html
	#
	# `-fno-pic` option it set for a workaround because GCC raised error for following reason:
	# `bootpack.c:(.text+0x9): undefined reference to `_GLOBAL_OFFSET_TABLE_`
	# PIC: Programmable Interrupt Controller, 割り込みコントローラ．
	$(GCC) $(CFLAGS) -T har.lds $(FILES) -o bootpack.hrb -fno-pic -Wl,-Map=bootpack.map

$(OSNAME).sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > $(OSNAME).sys

$(OSNAME).img: ipl10.bin $(OSNAME).sys $(APPS) Makefile
	# 1440 => フロッピーディスクの容量
	mformat -f 1440 -C -B ipl10.bin -i $(OSNAME).img ::
	mcopy $(OSNAME).sys $(APPS) -i $(OSNAME).img ::

vdi: $(OSNAME).img Makefile
	vboxmanage convertfromraw --format VDI $(OSNAME).img $(OSNAME).vdi

asm:
	make -r ipl10.bin

img:
	make -r $(OSNAME).img

run: img
	qemu-system-i386 -m 32 -enable-kvm -fda $(OSNAME).img

clean:
	rm *.lst *.bin *.sys *.img *.hrb *.o

src_only:
	ls | grep -vE '*.txt|*.c|*.h|*.asm|*.lds|Makefile|*.md' | xargs rm
