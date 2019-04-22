bits 32
	MOV  AL,'h'
	CALL 2*8:0x4e3b  ; @see bootpack.map
	MOV  AL,'e'
	CALL 2*8:0x4e3b
	MOV  AL,'l'
	CALL 2*8:0x4e3b
	MOV  AL,'l'
	CALL 2*8:0x4e3b
	MOV  AL,'l'
	CALL 2*8:0x4e3b
	MOV  AL,'o'
	CALL 2*8:0x4e3b
	RETF
