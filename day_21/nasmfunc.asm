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
GLOBAL  asm_inthandler20, asm_inthandler21, asm_inthandler27, asm_inthandler2c
GLOBAL  asm_hrb_api
GLOBAL  start_app
GLOBAL  load_cr0, store_cr0
GLOBAL  load_tr, farjmp
GLOBAL  memtest_sub
EXTERN  inthandler20, inthandler21, inthandler27, inthandler2c
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

asm_inthandler20:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     AX,SS
	CMP     AX,1*8
	JNE     .from_app

	MOV     EAX,ESP
	PUSH    SS
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler20
	ADD     ESP,8
	POPAD
	POP     DS
	POP     ES
	IRETD

.from_app:
; アプリが動いてるときに割り込まれた
	MOV     EAX,1*8
	MOV     DS,AX
	MOV     ECX,[0xfe4]
	ADD     ECX,-8
	MOV     [ECX+4],SS
	MOV     [ECX],ESP
	MOV     SS,AX
	MOV     ES,AX
	MOV     ESP,ECX
	CALL    inthandler20
	POP     ECX
	POP     EAX
	MOV     SS,AX
	MOV     ESP,ECX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler21:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     AX,SS
	CMP     AX,1*8
	JNE     .from_app

	MOV     EAX,ESP
	PUSH    SS
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler21
	ADD     ESP,8
	POPAD
	POP     DS
	POP     ES
	IRETD

.from_app:
; アプリが動いてるときに割り込まれた
	MOV     EAX,1*8
	MOV     DS,AX
	MOV     ECX,[0xfe4]
	ADD     ECX,-8
	MOV     [ECX+4],SS
	MOV     [ECX],ESP
	MOV     SS,AX
	MOV     ES,AX
	MOV     ESP,ECX
	CALL    inthandler21
	POP     ECX
	POP     EAX
	MOV     SS,AX
	MOV     ESP,ECX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler27:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     AX,SS
	CMP     AX,1*8
	JNE     .from_app

	MOV     EAX,ESP
	PUSH    SS
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler27
	ADD     ESP,8
	POPAD
	POP     DS
	POP     ES
	IRETD

.from_app:
; アプリが動いてるときに割り込まれた
	MOV     EAX,1*8
	MOV     DS,AX
	MOV     ECX,[0xfe4]
	ADD     ECX,-8
	MOV     [ECX+4],SS
	MOV     [ECX],ESP
	MOV     SS,AX
	MOV     ES,AX
	MOV     ESP,ECX
	CALL    inthandler27
	POP     ECX
	POP     EAX
	MOV     SS,AX
	MOV     ESP,ECX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_inthandler2c:
	PUSH    ES
	PUSH    DS
	PUSHAD
	MOV     AX,SS
	CMP     AX,1*8
	JNE     .from_app

	MOV     EAX,ESP
	PUSH    SS
	PUSH    EAX
	MOV     AX,SS
	MOV     DS,AX
	MOV     ES,AX
	CALL    inthandler2c
	ADD     ESP,8
	POPAD
	POP     DS
	POP     ES
	IRETD

.from_app:
; アプリが動いてるときに割り込まれた
	MOV     EAX,1*8
	MOV     DS,AX
	MOV     ECX,[0xfe4]
	ADD     ECX,-8
	MOV     [ECX+4],SS
	MOV     [ECX],ESP
	MOV     SS,AX
	MOV     ES,AX
	MOV     ESP,ECX
	CALL    inthandler2c
	POP     ECX
	POP     EAX
	MOV     SS,AX
	MOV     ESP,ECX
	POPAD
	POP     DS
	POP     ES
	IRETD

asm_hrb_api:
	; 最初から割り込み禁止
	PUSH    DS
	PUSH    ES
	PUSHAD  ; 保存のためのPUSH
	MOV     EAX,1*8
	MOV     DS,AX         ; DSだけOS用に
	MOV     ECX,[0xfe4]   ; OSのESP
	ADD     ECX,-40
	MOV     [ECX+32],ESP  ; アプリのESPを保存
	MOV     [ECX+36],SS   ; アプリのSSを保存
	; PUSHAD した値をシステムのスタックにコピー
	MOV     EDX,[ESP]
	MOV     EBX,[ESP+4]
	MOV     [ECX],EDX     ; hrb_apiに渡すため
	MOV     [ECX+4],EBX   ; hrb_apiに渡すため
	MOV     EDX,[ESP+8]
	MOV     EBX,[ESP+12]
	MOV     [ECX+8],EDX   ; hrb_api に渡すため
	MOV     [ECX+12],EBX  ; hrb_apiに渡すため
	MOV     EDX,[ESP+16]
	MOV     EBX,[ESP+20]
	MOV     [ECX+16],EDX  ; hrb_api に渡すため
	MOV     [ECX+20],EBX  ; hrb_apiに渡すため
	MOV     EDX,[ESP+24]
	MOV     EBX,[ESP+28]
	MOV     [ECX+24],EDX  ; hrb_api に渡すため
	MOV     [ECX+28],EBX  ; hrb_apiに渡すため

	MOV     ES,AX         ; 残りのセグメントレジスタもOS用にする
	MOV     SS,AX
	MOV     ESP,ECX
	STI

	CALL    hrb_api

	MOV     ECX,[ESP+32]  ; アプリのESPを思い出す
	MOV     EAX,[ESP+36]  ; アプリのSSを思い出す
	CLI
	MOV     SS,AX
	MOV     ESP,ECX
	POPAD
	POP     ES
	POP     DS
	IRETD   ; こいつは自動でSTIする

start_app:  ; void start_app(int eip, int cs, int esp, int ds)
	PUSHAD   ; 32 bit registerを全部保存しておく
	MOV     EAX,[ESP+36]  ; App用EIP
	MOV     ECX,[ESP+40]  ; App用CS
	MOV     EDX,[ESP+44]  ; App用ESP
	MOV     EBX,[ESP+48]  ; App用DS/SS
	MOV     [0xfe4],ESP   ; OS用のESP
	CLI      ; 取り換え中に割り込みが起きてほしくない
	MOV     ES,BX
	MOV     SS,BX
	MOV     DS,BX
	MOV     FS,BX
	MOV     GS,BX
	MOV     ESP,EDX
	STI      ; 完了
	PUSH    ECX       ; far-CALL のためにPUSH(cs)
	PUSH    EAX       ; far-CALL のためにPUSH(eip)
	CALL    FAR [ESP] ; アプリ呼び出し
; アプリが終了すると戻ってくる
	MOV     EAX,1*8   ; OS用のDS/SS
	CLI
	MOV     ES,AX
	MOV     SS,AX
	MOV     DS,AX
	MOV     FS,AX
	MOV     GS,AX
	MOV     ESP,[0xfe4]
	STI
	POPAD    ; 保存していたレジスタの回復
	RET

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
