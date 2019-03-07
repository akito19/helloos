# Day 1

## Terms

- IPL
  - Initial Program Loader
  - ブートセクタ．OS本体を読み込むプログラムを組み込む．

### Operations
- DB(data byte)
  - ファイルの内容を1バイトだけ直接書く
- DW(data word)
  - 1 word(16bit)
- DD(data double-word_)
  - 2 words(32bit)
- RESB: reserve byte


### 感想

binary editorでひたすら０を入力するの死ぬかと思ったが，`dd` するときに `count=2880` を指定してやれば勝手に期待するアドレスまで0を埋めてくれることに気がついた(http://hrb.osask.jp/wiki/?Linux/wako_memo)．だが１時間くらい０を押し続けるのに吸われた．
2880 はフロッピーディスクのサイズっぽい．時代だ．
