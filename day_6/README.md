# Day 6

### Header file

関数の宣言や `#define` とかだけを集めたファイル．
また，ヘッダファイル内にメモリアドレスを定義しておけば，あとから変更したくなったときにヘッダファイルだけを書き換えれば済む．

### `_load_gdtr` in nasmfunc.asm

```asm:nasmfunc.asm
load_gdtr:            ; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]  ; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET
```

GDTRはCPU内のGlobal Descriptor Tableのレジスタなので，`MOV` 命令では代入できず，直接メモリの番地を指定して代入する必要がある． `LGDT` 命令は指定したアドレスから6byte(= 48bit)を読み取って，GDTRレジスタに代入してくれる子．

さらに下位16bitはリミットを示している．
ここでの「リミット」は「GDTの有効バイト数 - 1」を意味していて，残りの上位32bitの部分で GDT が置かれた番地を示している．

### PIC 初期化

PIC: Programmable Interrupt Controller

CPUは設計上，割り込みは1つしかできないので，補助チップとしてチップセットに組み込まれた．

PICのレジスタはすべて8bit registerでありmaster-slave構成を取っている．ここで，マスタPICは IRQ 0-7番を，スレーブPICは IRQ 8-15番を担当し，このIRQが割り込み信号の監視を行う．

* IMR
  * Interrupt Mask Register
  * 割り込み設定を変更しているときに割り込みが起こると嫌なので，それを防ぐためにPICからマスクする
* ICW
  * Initial Control Word
  * 1-4のICWがあり合計4 byte．
  * ICW3 はマスタースレーブ接続に関する設定で，マスタに対してはどのIRQにスレーブが繋がっているのかを8 bitでせてい．
  * ICW2 はIRQをどの割り込み番号としてCPUに通知するかを設定するもので，OSをごとに設定する意味がある唯一のICW

### 割り込み処理

Ref: `nasmfunc.asm`

割り込み処理を実装するときにレジスタの値を愚直にPUSHして，処理を呼び出し，それが終わると今度はレジスタを元に戻して IRETD するのは，復帰後に処理を継続するため．もし割り込み復帰時にレジスタの値が狂っていたら，そこから処理が続行できなくなる．

また，`PUSHAD` は

```
PUSH EAX
PUSH ECX
PUSH EDX
PUSH EBX
PUSH ESP
PUSH EBP
PUSH ESI
PUSH EDI
```

の8命令に相当し，`POPAD` はその逆順にPOPする命令．
