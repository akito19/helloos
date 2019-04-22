# Day 20

### 1. プログラムの整理

リファクタ

### 2. 一文字表示API

`CALL` 命令は `JMP` 命令とほぼ同じ．ただし， `RET` 命令によって戻ってくるために， `CALL` にはスタックに戻り先のメモリ番地をPUSHする必要がある．

```asm
MOV   AL,'A'
CALL  `some memory address...`
```


