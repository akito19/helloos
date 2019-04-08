# Day 2

### Terms

#### 命令

- ORG
  - origin
  - 機械語実行時に，メモリ上のどのアドレスに読み込まれるかをアセンブラに教える
  - `$` も出力ファイルの何バイト目にあるかではなく読み込まれる予定のメモリ番地になる
- JMP
  - jump
- MOV
  - move
  - 代入命令
- ADD
- CMP
  - compare
- JE
  - jump if equal
  - 条件ジャンプ
- INT
  - interrupt
  - 割り込み命令
- HLT
  - halt
  - CPUを待機状態にする

#### レジスタ

計算機の算術命令実行時に利用する記憶素子．

以下は 16 bit のレジスタ

- AX
  - accumulator
- CX
  - counter
- DX
  - data
- BX
  - base
- SP
  - stack pointer
- BP
  - base pointer
- SI
  - source index
- DI
  - distination index
- ES
  - extra segment
- CS
  - code segment
- SS
  - stack segment
- DS
  - data segment
- FS
  - 特に名前がない
- GS
  - 特に名前がない

上4つのレジスタについては，8 bitのレジスタもある

- AH, AL
  - Accumulator High/Low
- CH, CL
  - Counter High/Low
- DH, DL
  - Data High/Low
- BH, BL
  - Base High/Low

たとえば，AXレジスタの上位8bitをAH, 下位8bitをALとして利用する．
また，SP, BP, SI, DI についてbitの上位下位でわけられていないので，Hight/Low で分けたくば，`MOV` のような代入命令で各々頑張る．

AXとかの prefix に E をつけると 32bit．

#### アセンブラ

label はただの数字であり，label名で `MOV` しても，そのメモリ番地が代入されるだけ．

`putloop` label先頭の `[SI]` はメモリ．MIPSでいうところのベースレジスタっぽい雰囲気．

```
MOV BYTE [678],123
```

とかやると，`[678]`番地に `123` (== `01111011`) を代入する．同様に

```
MOV WORD [678],123
```

の場合，123 は16bit として解釈されるので，`0000000001111011`について，下位8 bitが678番地へ，上位8 bitが679番地へ入る．メモリは上から下に伸びるので，下位と上位が入れ替わることはない．もし `DWORD` を指定した場合，32 bit(4 byte)分とる．

メモリ番地に上記のような定数ではなくレジスタを使用する場合，`BX`, `BP`, `SI`, `DI` のみ使うことができる．

#### その他

0xf0000番付近のアドレスはBIOSがいる，0番地近辺も同様にるらしい．UEFIも同じところにいる？
あと，件のメモリ番地(アドレス)のあたりにコードを置くと，BIOSと衝突が起きて事故るので注意．

#### 実行

```
% make     # create img file
% make vdi # create vdi file
% make run # run the image via qemu(x86_64)
```

とりあえずMakefileに書いてある

#### 注意

Makefileは死ぬほど雑にやったので，imgファイルとかが重複しても問答無用で何度もアセンブルする気がする．
真面目に対応するのをサボっている．
