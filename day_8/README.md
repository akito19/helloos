# Day 8

### マウス

マウスからのデータは3byteずつ送られてくる．

### 32bitモードへの道

```asm
; asmhead.asm

MOV  AL,0xff
OUT  0x21,AL
NOP
OUT  0xa1,AL
CLI
```

は

```c
io_out(PIC0_IMR, 0xff); // masterのすべての割り込み禁止
io_out(PIC1_IMR, 0xff); // slaveのすべての割り込み禁止
io_cli(); // CPUレベルでの割り込みも禁止
```

と等しい．つまり，CPUのモードを切り替えるときに割り込みされてほしくない．また，PICもあとから初期化するので，そのプロセス中に割り込みがくるのを防ぎたい．

`NOP`命令はCPUの1クロック分何もしない命令．

```asm
; asmhead.asm

CALL	waitkbdout
MOV		AL,0xd1
OUT		0x64,AL
CALL	waitkbdout
MOV		AL,0xdf
OUT		0x60,AL
CALL	waitkbdout
```

は `init_keyboard` と同様で，キーボードコントローラにコマンドを送る．
`0xdf` を A20GATE 信号線をONにする．この信号線はメモリの1MB以上の部分を使えるようにするための信号線．

```asm
; asmhead.asm
; [INSTRSET "i486p"] -> i486の命令で使いたい
LGDT  [GDTR0]         ; 暫定GDT
MOV   EAX,CR0
AND   EAX,0x7fffffff  ; ページング禁止のため，bit31に0にする．
OR    EAX,0x00000001  ; プロテクトモード移行のため，bit0を1にする．
MOV   CR0,EAX
JMP   pipelineflush

pipelineflush:
    MOV		AX,1*8
    MOV		DS,AX
    MOV		ES,AX
    MOV		FS,AX
    MOV		GS,AX
    MOV		SS,AX
```

OS以外がいじっていはいけない32bitレジスタ `CR0` にEAXを代入．また，GDTを使うようにすることでセグメントレジスタのアドレスの解釈が16bitモードから切り替わる．
また，プロテクトモードになることでアプリケーションがセグメントの設定を変えられなくなるほか，アプリケーションがOS用のセグメントを使うこともできなくなる．

`CR0`レジスタへの代入を行った直後に `JMP` を実行したのは，プロテクトモード移行によって機械語解釈が変わるため，以降の命令を改めてやり直してもらうため．

```asm
; asmhead.asm
pipelineflush:

    ...

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
```

これはつまるところメモリコピーを行う `memcpy` を呼び出していて，`memcpy(転送元番地, 転送先番地, 転送サイズ)` という関数に見立てると，上から

```c
memcpy(bootpack,    BOTPAK,     512*1024/4);
memcpy(0x7c00,      DSKCAC,     512/4);
memcpy(DSKCAC0+512, DSKCAC+512, cyls * 512*18*2/4 - 512/4);
```

転送サイズは double-word なので，バイト数を4で割って算出．

```c
memcpy(0x7c00, DSKCAC, 512/4);
```

の場合，`0x7c00` からの512byteを `0x00100000` 番地(DSKCAC)へコピーする．

```c
memcpy(bootpack, BOTPAK, 512*1024/4);
```

は `bootpack`番地からの 512KB を `BOTPAK`，つまり `0x00280000` 番地へコピーすることで， bootpack.hrb をコピーするための命令．

```asm
pipelineflush:
    ....

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
    MOV   ESP,[EBX+12]     ; スタック初期値
    JMP   DWORD 2*8:0x0000001b
```

bootpack.hrb のヘッダを解析して，データを転送する処理．`SHR` は右シフト命令．2進数の場合，右に1桁ずらすと値は1/2になり，左に1桁ずらすと2倍になる．この場合は2桁右シフトしているので，値は1/4．
`JZ` は `Jump if Zero`．値がゼロのときjumpする．

```asm
waitkbdout:
    IN		AL,0x64
    AND		AL,0x02
    IN    AL,0x60
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

    ALIGNB 16
```

`waitkbdout` は `wait_KBC_sendready` と同じだが，キーコードやマウスからのデータがあれば，受け取るために `0x60` から `IN` する．
`ALIGNB` はきりがよくなるまで `DB 0` を入れる命令．この場合，メモリアドレスが16で割り切れるようになるまで `DB 0` を実行する．

```asm
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
```

`GDTR0` は `GDT` の存在を知らせる．

#### はりぼてOSのメモリマップ

```
0x00000000 - 0x000fffff : 起動中に色々使うが，その後は空き(1MB)
0x00100000 - 0x00267fff : フロッピーディスクの内容記憶用(1440KB)
0x00268000 - 0x0026f7ff : あき(30KB)
0x0026f800 - 0x0026ffff : IDT(2KB)
0x00270000 - 0x0027ffff : GDT(64KB)
0x00280000 - 0x002fffff : bootpack.hrb(512KB)
0x00300000 - 0x003fffff : スタックなど(1MB)
0x00400000 -            : あき
```

メモリマップを予め作っておくと，OS開発時にアドレスの指定先を考えやすくなる．
