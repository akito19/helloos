;hello-os
;TAB = 4

CYLS	EQU		10     ; => CYLS=10 シリンダをどこまで読み込むか

	ORG		0x7c00     ; ブートセクタが読み込まれるアドレスの１つ

; Description for FAT12 format floppy
	JMP		entry
	DB		0x90
	DB		"HARIBOTE"
	DW		512
	DB		1
	DW		1
	DB		2
	DW		224
	DW		2880
	DB		0xf0
	DW		9
	DW		18
	DW		2
	DD		0
	DD		2880

	; FAT 12/16 におけるオフセット36以降のフィールド
	DB		0x00
	DB		0x00
	DB		0x29

	DD		0xffffffff
	DB		"HariboteOS "
	DB		"FAT12   "
	; See: http://hrb.osask.jp/wiki/?tools/nask
	; RESB	18
	TIMES 18 DB 0

; Boot sector
entry:
	MOV		AX,0        ; MOV means substitution.
	MOV		SS,AX       ; AX, SS, SP, DS, ES are registers.
	MOV		SP,0x7c00
	MOV		DS,AX
	MOV		ES,AX

	MOV		AX,0x0820
	MOV		ES,AX
	MOV		CH,0        ; シリンダ0
	MOV		DH,0        ; ヘッド0
	MOV		CL,2        ; セクタ2

readloop:
	MOV		SI,0        ; 失敗回数を数えるレジスタ

retry:
	MOV		AH,0x02     ; AH = 0x02 : ディスク読み込み
	MOV		AL,1        ; 1 セクタ
	MOV		BX,0        ; ES = 0x0820, BX = 0 なので，このディスクが読み込まれるデータは 0x8200 番地から 0x83ff 番地
	MOV		DL,0x00     ; Aドライブ
	INT		0x13        ; ディスクBIOS呼び出し
	JNC		next         ; エラーが起きなければ next へ
	ADD		SI,1        ; SI に 1 を加算
	CMP		SI,5        ; SI と 5 を比較
	JAE		error       ; SI >= 5 ならば error へ
	MOV		AH,0x00     ; この行から3行はエラー時に実行するシステムリセット関連
	MOV		DL,0x00     ; Aドライブ
	INT		0x13        ; ドライブのリセット
	JMP		retry

next:
	MOV		AX,ES       ; アドレスを 0x200 進める. 0x20 == 512 / 16 の16進表記
	ADD		AX,0x0020
	MOV		ES,AX       ; ADD ES,0x020 命令がないので代替らしい
	ADD		CL,1        ; 次のセクタを読み込むために 1 を加算
	CMP		CL,18
	JBE   readloop    ; CL <= 18 ならば readloop へ
	MOV		CL,1
	ADD		DH,1
	CMP		DH,2
	JB		readloop    ; DH < 2 ならば readloop へ
	MOV		DH,0
	ADD		CH,1
	CMP		CH,CYLS
	JB		readloop    ; CH < CYLS ならば readloop へ

	MOV		[0x0ff0],CH ; 0x0ff0 のアドレスにシリンダのカウント数を書き込む(どこまでディスクを書き込んだのか)
	JMP		0xc200      ; OS 本体の実行

error:
	MOV		SI,msg

putloop:
	MOV		AL,[SI]     ; MOV AL, BYTE[SI] ともかけるが，AL が4 bitであり，bit数を揃えなければならないので，BYTE は省略.
	ADD		SI,1
	CMP		AL,0
	JE		fin         ; => if (AL == 0) { goto fin }
	MOV		AH,0x0e     ; AH = 0x0e
	MOV		BX,15       ; BH = 0; BL = color code
	INT		0x10        ; Call the function where is in `0x10` for operate video card from BIOS.
	JMP		putloop

fin:
	HLT
	JMP		fin         ; Infinit loop

; Except boot sector
msg:
	DB		0x0a,0x0a
	DB		"Load error"
	DB		0x0a
	DB		0
	; RESB	0x7dfe-0x7c00-($-$$)  ; 現在地から 0x1fd までの未使用領域を0で埋める. 7c00はブートセクタのスタートなので，その分を引く
	; See: https://nasm.us/doc/nasmdoc3.html
	TIMES 0x1fe-($-$$) DB 0

; BS_BootCode
	DB		0x55,0xaa
