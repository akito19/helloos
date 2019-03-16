OSNAME := haribote

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

bootpack.hrb: bootpack.c har.lds nasmfunc.o Makefile
	# http://softwaretechnique.jp/OS_Development/startup_kernel.html
	#
	# `-fno-pic` option it set for a workaround because GCC raised error for following reason:
	# `bootpack.c:(.text+0x9): undefined reference to `_GLOBAL_OFFSET_TABLE_`
	# PIC: Programmable Interrupt Controller, 割り込みコントローラ．
	gcc -march=i486 -m32 -nostdlib -T har.lds -g bootpack.c nasmfunc.o -o bootpack.hrb -fno-pic

$(OSNAME).sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > $(OSNAME).sys

$(OSNAME).img: ipl10.bin $(OSNAME).sys Makefile
	# 1440 => フロッピーディスクの容量
	mformat -f 1440 -C -B ipl10.bin -i $(OSNAME).img ::
	mcopy $(OSNAME).sys -i $(OSNAME).img ::

vdi: $(OSNAME).img Makefile
	VboxManage convertfromraw --format VDI $(OSNAME).img $(OSNAME).vdi

asm:
	make -r ipl10.bin

img:
	make -r $(OSNAME).img

run:
	make img
	qemu-system-i386 -fda $(OSNAME).img

clean:
	rm *.lst *.bin *.sys *.img *.hrb *.o

src_only:
	ls | grep -vE '*.c|*.asm|*.lds|Makefile|README.md' | xargs rm
