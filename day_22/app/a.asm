bits 32

GLOBAL api_putchar
GLOBAL api_end
GLOBAL api_putstr0

section .text

api_putchar:
	mov  edx,1
	mov  al,[esp+4]
	int  0x40
	ret

api_end:
	mov  edx,4
	int  0x40

api_putstr0:
	push  ebx
	mov   edx,2
	mov   ebx,[esp+8]
	int   0x40
	pop   ebx
	ret
