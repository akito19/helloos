.SUFFIXES: .asm .bin
.SUFFIXES: .c .o
OSNAME = haribote
CFLAGS = -march=i486 -m32 -nostdlib -fno-pic -c
APPS := $(wildcard ./app/*.hrb)
UTIL := ../utils

default:
	make img

nasmfunc.o: nasmfunc.asm Makefile
	nasm -f elf -o nasmfunc.o nasmfunc.asm

.asm.bin:
	nasm $< -o $@ -l $(*F).lst

.c.o:
	gcc $(CFLAGS) $< -o $@

FILES = bootpack.o nasmfunc.o graphic.o dsctbl.o int.o \
				fifo.o keyboard.o mouse.o memory.o sheet.o \
				timer.o mtask.o window.o console.o file.o \
				hankaku.o mysprintf.o strncmp.o
bootpack.bin: $(FILES) har.lds Makefile
	ld -m elf_i386 -fno-pie -shared -e HariMain -T har.lds -o $@ $(FILES)

$(OSNAME).sys: asmhead.bin bootpack.bin Makefile
	cat asmhead.bin bootpack.bin > $(OSNAME).sys

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
