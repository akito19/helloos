bits 32
	MOV  AL,'A'
	CALL 2*8:0x4e3b  ; @see bootpack.map
fin:
	HLT
	JMP  fin
