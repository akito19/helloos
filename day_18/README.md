# Day 18

### 1,2. カーソル点滅制御

ウィンドウ切り替え時にカーソルの点滅も切り替える．

### 3. コンソールでの改行対応

### 4. スクロールに対応

Enterが押されたら，コンソール下部をずらして，最下段を黒く塗りつぶす

### 5. mem コマンド

今まで表示しっぱなしにしていたメモリ数表示をコマンド化

### 6. cls コマンド

コンソールをクリアする

### 7. dir コマンド

Linux でいうところの `ls`．

filetype は

```
0x01: Read only files
0x02: hidden files
0x04: System files
0x08: Information not but files (e.g. disk name)
0x10: Directories
```

読み込み専用かつ隠しファイルみたいな場合は，それぞれを足して `0x03` とかになる．
