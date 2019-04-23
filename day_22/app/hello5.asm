bits 32

GLOBAL HariMain

section .text

HariMain:
	mov   edx,2
	mov   ebx,msg
	int   0x40
	mov   edx,4
	int   0x40

section .data
msg:
	db   "hello, world",0x0a,0
