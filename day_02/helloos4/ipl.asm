;hello-os
;TAB = 4

	ORG		0x7c00    ; ブートセクタが読み込まれるアドレスの１つ

; Description for FAT12 format floppy
	JMP		entry
	DB		0x90
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
entry:
	MOV		AX,0       ; MOV means substitution.
	MOV		SS,AX      ; AX, SS, SP, DS, ES are registers.
	MOV		SP,0x7c00
	MOV		DS,AX
	MOV		ES,AX

	MOV		SI,msg

putloop:
	MOV		AL,[SI]    ; MOV AL, BYTE[SI] ともかけるが，AL が4 bitであり，bit数を揃えなければならないので，BYTE は省略.
	ADD		SI,1
	CMP		AL,0
	JE		fin        ; => if (AL == 0) { goto fin }
	MOV		AH,0x0e    ; AH = 0x0e
	MOV		BX,15      ; BH = 0; BL = color code
	INT		0x10       ; Call the function where is in `0x10` for operate video card from BIOS.
	JMP		putloop

fin:
	HLT
	JMP		fin        ; Infinit loop

msg:
	DB		0x0a,0x0a
	DB		"Hello, world!"
	DB		0x0a,0x0a
	DB		"I like Milky Holmes."
	DB		0x0a
	DB		0
	RESB	0x1fe-($-$$)
	DB		0x55,0xaa
