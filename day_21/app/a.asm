bits 32

GLOBAL api_putchar
GLOBAL api_end

section .text

api_putchar:
	mov  edx,1
	mov  al,[esp+4]
	int  0x40
	ret

api_end:
	mov  edx,4
	int  0x40
