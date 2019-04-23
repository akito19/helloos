bits 32

GLOBAL api_putchar
GLOBAL api_end
GLOBAL api_putstr0
GLOBAL api_openwin

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

api_openwin:
	push  edi
	push  esi
	push  ebx
	mov   edx,5
	mov   ebx,[esp+16] ; buf
	mov   esi,[esp+20] ; xsize
	mov   edi,[esp+24] ; ysize
	mov   eax,[esp+28] ; col_inv
	mov   ecx,[esp+32] ; title
	int   0x40
	pop   ebx
	pop   esi
	pop   edi
	ret
