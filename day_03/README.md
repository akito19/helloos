# Day 3

## 命令

- JC
  - jump if carry
  - carry flag == 1 のときに jump
- JC
  - jump if not carry
  - carry flag != 1 のときに jump
- JAE
  - jump if above or equal
  - 与条件以上の値ならば jump
  - slt, beq とかを組み合わせる MIPS より直感的では
- JBE
  - jump if below or equal
  - 与条件以下の値ならばjump
- JB
  - jump if below
  - 与条件未満の値ならばjump
- HLT
  - halt
  - 命令の実行を停止し，プロセッサをHALT状態にする．


## Disk I/O, verfy or seek sector

| Register | Read disk | Write disk | Verify sector | Seek sector |
| -------- | --------- | ---------- | ------------- | ----------- |
| AH       | 0x02      | 0x03       | 0x04          | 0x0c        |

| Register | 役割 |
| -------- | ---- |
| AL       | 処理するセクタ数(連続したセクタを処理できる)     |
| CH       | シリンダ番号 & 0xff                              |
| CL       | セクタ番号(bit0-5), (シリンダ番号* & 0x3000) >> 2 |
| DH       | ヘッド番号                                       |
| DL       | ドライブ番号                                     |
| ES:BX    | バッファアドレス                                 |
| FLAGS.CF == 0 | 戻り値，エラーなし，AH == 0                 |
| FLAGS.CF == 1 | 戻り値，エラーあり，AHにエラーコード        |

FLAGS.CF == `carry flag`
flag == 1ビットしか記憶できないレジスタ

#### シリンダ(cylinder)

フロッピーディスクの場合，片面80本(0-79番)からなる．

#### ヘッド

フロッピーディスクの場合磁気ヘッド．

#### セクタ

磁気記憶をするときにディスクの円上を分割する．フロッピーディスクの場合は1シリンダあたり 1 - 18 までの18セクタ．

よってフロッピーディスクでは1枚のディスクあたり80シリンダ，両面記録方式なのでヘッド2つ，それぞれについて18セクタあり，1セクタあたり512 byte なので

`80 * 2 * 18 * 512 = 1,474,560 byte == 1,440 KB`

小さい．

#### バッファアドレス

メモリのどこを読み込むか．

BIOS開発当初のCPUは32 bitレジスタをつけることができず，補助的な役目をするセグメントレジスタを作り，メモリ番地を指定するときにこのセグメントレジスタも使えるようにした．
また，セグメントレジスタはどんなメモリの番地指定に対しても，同時に指定しなければならない．
つまり， `MOV CX,[1234]` は事実上 `MOV CX,[DS:12345]` を表す．

## 画面モード

Video BIOS の AH=0x00 から指定可能．

| Register | Mode | Description |
| -------- | ---- | ----------- |
| AH       | 0x00 |             |
| AL       | 0x03 | 16色テキスト， 80x25 |
| AL       | 0x12 | VGA graphics, 640x480x4bit color, 独自プレーンアクセス |
| AL       | 0x13 | VGA graphics, 320x200x8bit color, パックドピクセル |
| AL       | 0x6a | 拡張VGA graphics, 800x600x4bit color, 独自プレーンアクセス |

戻り値: なし．
