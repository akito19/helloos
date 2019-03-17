# Day 5

### 構造体

意味のあるデータの塊．

```c
// Example

void HariMain(void)
{
  struct BOOTINFO abc;

  abc.scrnx = 320;
  abc.scrny = 200;
  abc.vram = 0xa0000;
  ...
}
```

今回のケースだと，ポインタとして利用するので

```c
// 32bit なので，メモリアドレスは4byteで示されるため， binfo も 4byte の変数
struct BOOTINFO *binfo;

...

// ポインタを利用するので，メモリ番地をセット
binfo = (struct BOOTINFO *) 0x0ff0;

...

xsize = (*binfo).scrnx;
```

また，構造体へのアクセス方法は

```c
xsize = (*binfo).scrnx;
```

のような書き方の他に

```c
xsize = binfo->scrnx;
```

のようにも書ける．

### GDT/IDT

* GDT
  * Global (segment) Descriptor Table
  * 大域セグメント記述子表
* IDT
  * Interrupt Descriptor Table
  * 割り込み記述子表

GDT, IDTはともにCPUへの設定．

#### Segmentation

複数のプログラムが同時に実行されるとき，メモリの利用範囲（メモリ番地の競合）が重なってしまうことを防ぐ．
そのために，メモリを任意にいくつかのブロックに切り分けて，各ブロックの最初の番地を 0 として扱えるようにした機能．これにより，どのプログラムであっても `ORG 0` みたいな始め方ができる．

セグメントの1つを表すのに必要な情報は以下:

* セグメントの大きさはどのくらいか
* セグメントがどの番地から始まるか
* セグメントの管理用属性（書き込み禁止，実行禁止，システム専用，etc.）

また，セグメントレジスタは 16bit なので，このレジスタにはセグメント番号を入れておき，各セグメント番号に対応するセグメントを設定する．
なお，CPUの仕様上セグメントレジスタの下位3bitは使用できないので，実質使用可能なのは13bit，つまり 0 - 8,191 の範囲．
設定には 65,536 byte(= 64KB, 8192 segments * 8)が必要で，これをメモリに書き込むことをGDT．

GDT は CPU の GDTR(Global Descriptor Table Register) に先頭番地と有効設定個数を設定する．

IDT は割り込み制御．割り込みは，CPUのアイドリング状態を防ぐためにあり，各デバイスの状態に変化があったときに都度割り込み命令を発生させる一方，変化がない状態のときは，デバイスの監視はせず，通常の演算処理にリソースを回せるようになる．
