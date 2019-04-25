bits 32

GLOBAL api_putchar, api_putstr0
GLOBAL api_end, api_point
GLOBAL api_openwin, api_putstrwin, api_boxfilwin
GLOBAL api_refreshwin, api_linewin
GLOBAL api_initmalloc, api_malloc

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

api_refreshwin:
	push edi
	push esi
	push ebx
	mov  edx,12
	mov  ebx,[esp+16] ; win
	mov  eax,[esp+20] ; x0
	mov  ecx,[esp+24] ; y0
	mov  esi,[esp+28] ; x1
	mov  edi,[esp+32] ; y1
	int  0x40
	pop  ebx
	pop  esi
	pop  edi
	ret

api_linewin:
	push edi
	push esi
	push ebp
	push ebx
	mov  edx,13
	mov  ebx,[esp+20] ; win
	mov  eax,[esp+24] ; x0
	mov  ecx,[esp+28] ; y0
	mov  esi,[esp+32] ; x1
	mov  edi,[esp+36] ; y1
	mov  ebp,[esp+40] ; col
	int  0x40
	pop  ebx
	pop  ebp
	pop  esi
	pop  edi
	ret

api_initmalloc:
	push  ebx
	mov   edx,8
	mov   ebx,[cs:0x0020] ; malloc 領域の番地
	mov   eax,ebx
	add   eax,32*1024
	mov   ecx,[cs:0x0000] ; data segment の大きさ
	sub   ecx,eax
	int   0x40
	pop   ebx
	ret

api_malloc:
	push  ebx
	mov   edx,9
	mov   ebx,[cs:0x0020]
	mov   ecx,[esp+8]     ; size
	int   0x40
	pop   ebx
	ret

api_free:
	push  ebx
	mov   edx,10
	mov   ebx,[cs:0x0020]
	mov   eax,[esp+8]
	mov   ecx,[esp+12]
	int   0x40
	pop   ebx
	ret

api_point:
	push  edi
	push  esi
	push  ebx
	mov   edx,11
	mov   ebx,[esp+16]  ; win
	mov   esi,[esp+20]  ; x
	mov   edi,[esp+24]  ; y
	mov   eax,[esp+28]  ; col
	int   0x40
	pop   ebx
	pop   esi
	pop   edi
	ret
