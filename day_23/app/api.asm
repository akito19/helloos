bits 32

GLOBAL api_putchar
GLOBAL api_end
GLOBAL api_putstr0
GLOBAL api_openwin
GLOBAL api_putstrwin
GLOBAL api_boxfilwin

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

api_putstrwin:
	push  edi
	push  esi
	push  ebp
	push  ebx
	mov   edx,6
	mov   ebx,[esp+20] ; win
	mov   esi,[esp+24] ; x
	mov   edi,[esp+28] ; y
	mov   eax,[esp+32] ; col
	mov   ecx,[esp+36] ; len
	mov   ebp,[esp+40] ; str
	int   0x40
	pop   ebx
	pop   ebp
	pop   esi
	pop   edi
	ret

api_boxfilwin:
	push  edi
	push  esi
	push  ebp
	push  ebx
	mov   edx,7
	mov   ebx,[esp+20] ; win
	mov   eax,[esp+24] ; x0
	mov   ecx,[esp+28] ; y0
	mov   esi,[esp+32] ; x1
	mov   edi,[esp+36] ; y1
	mov   ebp,[esp+40] ; col
	int   0x40
	pop   ebx
	pop   ebp
	pop   esi
	pop   edi
	ret
