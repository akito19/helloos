CFLAGS = -march=i486 -m32 -nostdlib -fno-pic -c

api.o: api.asm
	nasm -f elf -o api.o api.asm

%.o: %.asm
	nasm $< -o $@ -l $(*F).lst

%.o: %.c
	gcc $(CFLAGS) -o $@ $<

%.hrb: %.o api.o
	ld -fno-pie -shared -e HariMain -T app.lds -m elf_i386 -o $@ $< api.o
