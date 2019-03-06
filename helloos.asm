;hello-os
;TAB = 4

; Terms
;   IPL: Initial Program Loader...ブートセクタ．OS本体を読み込むプログラムを組み込む．
; Operations
;   DB: data byte...ファイルの内容を1バイトだけ直接書く
;   DW: data word...1 word(16bit)
;   DD: data double-word...2 words(32bit)
;   RESB: reserve byte

; Description for FAT12 format floppy
	DB		0xeb,0x4e,0x90
	DB		"HELLOIPL"
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
	DB		0,0,0x29
	DD		0xffffffff
	DB		"HELLO-OS   "
	DB		"FAT12   "
	RESB	18

; Main program
	DB		0xb8,0x00,0x00,0x8e,0xd0,0xbc,0x00,0x7c
	DB		0x8e,0xd8,0x8e,0xc0,0xbe,0x74,0x7c,0x8a
	DB		0x04,0x83,0xc6,0x01,0x3c,0x00,0x74,0x09
	DB		0xb4,0x0e,0xbb,0x0f,0x00,0xcd,0x10,0xeb
	DB		0xee,0xf4,0xeb,0xfd

; Messages
	DB		0x0a,0x0a
	DB		"Hello, world!"
	DB		0x0a
	DB		0
	RESB	0x1fe-($-$$)
	DB		0x55,0xaa

; ブートセクタ以外の部分
	DB		0xf0,0xff,0xff,0x00,0x00,0x00,0x00,0x00
	RESB	4600
	DB		0xf0,0xff,0xff,0x00,0x00,0x00,0x00,0x00
	RESB	1469432
