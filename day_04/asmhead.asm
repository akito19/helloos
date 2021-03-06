BOTPAK  EQU	0x00280000    ; bootpackのロード先
DSKCAC  EQU	0x00100000    ; ディスクキャッシュの場所
DSKCAC0 EQU	0x00008000    ; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO 関係
CYLS 	EQU		0x0ff0      ; ブートセクタが設定する
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2      ; 色数に関する情報．何ビットカラーか
SCRNX	EQU		0x0ff4      ; 解像度のX
SCRNY	EQU		0x0ff6      ; 解像度のY
VRAM	EQU		0x0ff8      ; video RAM, Graphic buffer の開始番地

			ORG		0xc200      ; disk image上の 0x004200 に相当する位置でプログラムを実行する. ref haribote.img
									      ; boot sector 0x8000 番地に来るように，ディスクをメモリに読み込んでいるので，
									      ; ORG <- 0x8000 + 0x4200
			MOV		AL,0x13     ; VGA graphics, 320x200x8bit color, 画面モード切り替え
			MOV		AH,0x00
			INT		0x10

			MOV		BYTE [VMODE],8   ; 画面モードをメモする
			MOV		WORD [SCRNX],320
			MOV		WORD [SCRNY],200
			MOV		DWORD [VRAM],0x000a0000

			; キーボードのLED状態をBIOSに教えてもらう
			; AL == 状態コード
			;     bit0 : Right shift
			;     bit1 : Left shift
			;     bit2 : Ctrl
			;     bit3 : Alt
			;     bit4 : ScrollLock
			;     bit5 : NumLock
			;     bit6 : CapsLock
			;     bit7 : Insert Mode
			MOV		AH,0x02     ; キーロック&シフト状態取得
			INT		0x16        ; keyboard BIOS
			MOV		[LEDS],AL

			MOV		AL,0xff     ; PICが一切の割り込みを受け付けないように
			OUT		0x21,AL
			NOP
			OUT		0xa1,AL
			CLI               ; CPUレベルでの割り込みも禁止する

			; CPUから1MB以上のメモリにアクセスできるように，A20GATEを設定
			CALL	waitkbdout
			MOV		AL,0xd1
			OUT		0x64,AL
			CALL	waitkbdout

			; protect mode
			; [INSTRSET "i486p"] -> i486の命令で使いたい
			; ここで指定せず，gcc compile 時に指定
			LGDT	[GDTR0]         ; 暫定GDT
			MOV		EAX,CR0
			AND		EAX,0x7fffffff  ; ページング禁止のため，bit31に0にする．
			OR		EAX,0x00000001  ; プロテクトモード移行のため，bit0を1にする．
			MOV		CR0,EAX
			JMP		pipelineflush

pipelineflush:
			MOV		AX,1*8      ; 読み書き可能セグメント 32 bit
			MOV		DS,AX
			MOV		ES,AX
			MOV		FS,AX
			MOV		GS,AX
			MOV		SS,AX

			; bootpackの転送
			MOV		ESI,bootpack  ; 転送元
			MOV		EDI,BOTPAK    ; 転送先
			MOV		ECX,512*1024/4
			CALL	memcpy

			; disk dataを本来の位置へ転送

			; Boot sector
			MOV		ESI,0x7c00    ; 転送先
			MOV		EDI,DSKCAC    ; 転送先
			MOV		ECX,512/4
			CALL	memcpy

			; Others
			MOV		ESI,DSKCAC0+512  ; 転送元
			MOV		EDI,DSKCAC+512   ; 転送先
			MOV		ECX,0
			MOV		CL,BYTE [CYLS]
			IMUL	ECX,512*18*2/4   ; シリンダ数からbyte数/4 に変換
			SUB		ECX,512/4        ; IPLの分だけ差し引く
			CALL	memcpy

			; asmhead でしなければいけないことは終わったので，
			; 残りは bootpack の仕事

			; bootpack の起動
			MOV		EBX,BOTPAK
			MOV		ECX,[EBX+16]
			ADD		ECX,3            ; ECX += 3;
			SHR		ECX,2            ; ECX /= 4;
			JZ		skip             ; 転送すべきものがない
			MOV		ESI,[EBX+20]     ; 転送元
			ADD		ESI,EBX
			MOV		EDI,[EBX+12]     ; 転送先
			CALL  memcpy

skip:
			MOV		ESP,[EBX+12]     ; スタック初期値
			JMP		DWORD 2*8:0x0000001b

waitkbdout:
			IN		AL,0x64
			AND		AL,0x02
			JNZ		waitkbdout      ; AND の結果が 0 でなければ waitbdout へ
			RET

memcpy:
			MOV		EAX,[ESI]
			ADD		ESI,4
			MOV		[EDI],EAX
			ADD		EDI,4
			SUB		ECX,1
			JNZ		memcpy          ; SUB の結果が 0 でなければ memcpy へ
			RET
			; memcpy はアドレスサイズプリフィクスを入れ忘れなければ，ストリング命令でも書ける
			ALIGNB 16

GDT0:
			RESB	8     ; null sector
			DW		0xffff,0x0000,0x9200,0x00cf   ; 読み書き可能セグメント 32 bit
			DW		0xffff,0x0000,0x9a28,0x0046   ; 実行可能セグメント 32bit(for bootpack)

			DW		0

GDTR0:
			DW		8*3-1
			DD		GDT0

			ALIGNB	16

bootpack:

