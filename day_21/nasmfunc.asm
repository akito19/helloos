; nasmfunc
; HLT命令はC言語で生成できないので，アセンブリ言語で対応
; http://hrb.osask.jp/wiki/?tools/nask
section .text     ; object fileでは，これを書いてからプログラムを書く

; このプログラムに含まれる関数名
GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
GLOBAL  io_in8, io_in16, io_in32
GLOBAL  io_out8, io_out16, io_out32
GLOBAL  io_load_eflags, io_store_eflags
GLOBAL  load_gdtr, load_idtr
GLOBAL  asm_inthandler20, asm_inthandler21, asm_inthandler27, asm_inthandler2c, asm_inthandler0d
GLOBAL  asm_hrb_api
GLOBAL  start_app
GLOBAL  load_cr0, store_cr0
GLOBAL  load_tr, farjmp
GLOBAL  memtest_sub
EXTERN  inthandler20, inthandler21, inthandler27, inthandler2c, inthandler0d
EXTERN  hrb_api

io_hlt:             ; void io_hlt(void); in C lang.
	HLT
	RET               ; return

io_cli:             ; void io_cli(void);
	CLI
	RET

io_sti:             ; void io_sti(void);
	STI
	RET

io_stihlt:          ; void io_stihlt(void);
	STI
	HLT
	RET

io_in8:             ; void io_in8(int port);
	MOV   EDX,[ESP+4] ; port
	MOV   EAX,0       ; 32bit で動かすから，足りない分は0で埋めるという処理??
	IN    AL,DX
	RET

io_in16:            ; void io_in16(int port);
	MOV   EDX,[ESP+4] ; port
	MOV   EAX,0
	IN    AX,DX
	RET

io_in32:            ; void io_in32(int port);
	MOV   EDX,[ESP+4] ; port
	IN    EAX,DX
	RET

io_out8:            ; void io_out8(int port, int data);
	MOV   EDX,[ESP+4] ; port
	MOV   AL,[ESP+8]  ; data
	OUT   DX,AL
	RET

io_out16:           ; void io_out16(int port, int data);
	MOV   EDX,[ESP+4] ; port
	MOV   EAX,[ESP+8] ; data
	OUT   DX,AX
	RET

io_out32:           ; void io_out32(int port, int data);
	MOV   EDX,[ESP+4] ; port
	MOV   EAX,[ESP+8] ; data
	OUT   DX,EAX
	RET

io_load_eflags:     ; int io_load_eflags(void);
	PUSHFD            ; PUSH EFLAGS
	POP   EAX
	RET

io_store_eflags:    ; void _store_eflags(int eflags);
	MOV	  EAX,[ESP+4]
	PUSH  EAX
	POPFD             ; POP EFLAGS
	RET

load_gdtr:          ; void load_gdtr(int limit, int addr);
	MOV   AX,[ESP+4]  ; limit
	MOV   [ESP+6],AX
	LGDT  [ESP+6]
	RET

load_idtr:          ; void load_idtr(int limit, int addr);
	MOV   AX,[ESP+4]  ; limit
	MOV   [ESP+6],AX
	LIDT  [ESP+6]
	RET

asm_inthandler0d:
	STI
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     EAX,ESP
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler0d
	CMP     EAX,0
	JNE     end_app
	POP     EAX
	POPAD
	POP     DS
	POP     ES
	ADD     ESP,4      ; INT 0x0d ではこれが必要
	IRETD

asm_inthandler20:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     EAX,ESP
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler20
	POP     EAX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler21:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     EAX,ESP
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler21
	POP     EAX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler27:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     EAX,ESP
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler27
	POP     EAX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler2c:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     EAX,ESP
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler2c
	POP     EAX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_hrb_api:
	STI
	PUSH    DS
	PUSH    ES
	PUSHAD  ; 保存のためのPUSH
	PUSHAD  ; hrb_api に渡すためにPUSH
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    hrb_api
	CMP     EAX,0    ; EAX が 0 でなければ終了処理
	JNE     end_app
	ADD     ESP,32
	POPAD
	POP     ES
	POP     DS
	IRETD

end_app:
; EAX は tss.esp0 の番地
	MOV     ESP,[EAX]
	POPAD
	RET

start_app:  ; void start_app(int eip, int cs, int esp, int ds, int tss_esp0)
	PUSHAD   ; 32 bit registerを全部保存しておく
	MOV     EAX,[ESP+36]  ; App用EIP
	MOV     ECX,[ESP+40]  ; App用CS
	MOV     EDX,[ESP+44]  ; App用ESP
	MOV     EBX,[ESP+48]  ; App用DS/SS
	MOV     EBP,[ESP+52]  ; tss.esp0 の番地
	MOV     [EBP],ESP     ; OS用ESPの保持
	MOV     [EBP+4],SS    ; OS用SSの保持
	MOV     ES,BX
	MOV     DS,BX
	MOV     FS,BX
	MOV     GS,BX
; 以下はRETFでアプリに行かせるためにスタック調整
	OR      ECX,3     ; アプリ用セグメント番号に3をORする
	OR      EBX,3     ; アプリ用セグメント番号に3をORする
	PUSH    EBX       ; アプリのSS
	PUSH    EDX       ; アプリのESP
	PUSH    ECX       ; アプリのCS
	PUSH    EAX       ; アプリのEIP
	RETF
; アプリが終了してもここには来ない

load_cr0:
	MOV     EAX,CR0
	RET

store_cr0:
	MOV     EAX,[ESP+4]
	MOV     CR0,EAX
	RET

load_tr:
	LTR     [ESP+4]     ; tr
	RET

farjmp:
	JMP     FAR [ESP+4] ; eip, cs
	RET

memtest_sub:  ; unsigned int memtest_sub(unsigned int start, unsigned int end)
	PUSH    EDI                    ; EBX, ESI, EDI も使いたい
	PUSH    ESI
	PUSH    EBX
	MOV     ESI,0xaa55aa55         ; pat0 = 0xaa55aa55
	MOV     EDI,0x55aa55aa         ; pat1 = 0x55aa55aa
	MOV     EAX,[ESP+12+4]         ; i = start;

mts_loop:
	MOV     EBX,EAX
	ADD     EBX,0xffc              ; p = i + 0xffc
	MOV     EDX,[EBX]              ; old = *p
	MOV     [EBX],ESI              ; *p = pat0
	XOR     DWORD [EBX],0xffffffff ; *p ^= 0xffffffff
	CMP     EDI,[EBX]              ; if (*p != pat1) goto fin;
	JNE     mts_fin
	XOR     DWORD [EBX],0xffffffff ; *p ^= 0xffffffff
	CMP     ESI,[EBX]              ; if (*p != pat0) goto fin;
	JNE     mts_fin
	MOV     [EBX],EDX              ; *p = old
	ADD     EAX,0x1000             ; i += 0x1000
	CMP     EAX,[ESP+12+8]         ; if (i <= end) goto mts_loop
	JBE     mts_loop
	POP     EBX
	POP     ESI
	POP     EDI
	RET

mts_fin:
	MOV     [EBX],EDX              ; *p = old
	POP     EBX
	POP     ESI
	POP     EDI
	RET
