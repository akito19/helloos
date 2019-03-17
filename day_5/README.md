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

