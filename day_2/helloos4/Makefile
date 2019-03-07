img:
	make -r helloos.img

# .lst file is the list file for assembly.
ipl.bin: ipl.asm Makefile
	nasm ipl.asm -l ipl.lst

helloos.img: ipl.bin Makefile
	nasm ipl.asm -o helloos.img -l ipl.lst

vdi: helloos.img Makefile
	VboxManage convertfromraw --format VDI helloos.img helloos.vdi

run:
	make img
	qemu-system-x86_64 helloos.img

clean:
	ls | grep -vE '*.img|*.vid|Makefile' | xargs rm

src_only:
	ls | grep -vE 'ipl.*|Makefile' | xargs rm
