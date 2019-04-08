; nasmfunc
; HLT命令はC言語で生成できないので，アセンブリ言語で対応
; http://hrb.osask.jp/wiki/?tools/nask

GLOBAL	_io_hlt ; このプログラムに含まれる関数名

[SECTION .text]     ; object fileでは，これを書いてからプログラムを書く
_io_hlt:          ; void io_hlt(void); in C lang.
		HLT
		RET           ; return
