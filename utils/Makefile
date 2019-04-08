GCC = gcc

hankaku.o: convert_hankaku.c Makefile
	$(GCC) -m32 -o hankaku.o convert_hankaku.c

hankaku.c: hankaku.o hankaku.txt Makefile
	./hankaku.o

clean:
	rm hankaku.o
