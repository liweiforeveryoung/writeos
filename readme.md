### 写操作系统咯

#### 一些魔数

###### 0x7c00

启动区（boot sector）的装载地址

#### 内存布局

内存的0号地址，也就是最开始的部分，是BIOS程序用来实现各种不同功能的地方，如果我们随便使用的话，就会与BIOS发生冲突，结果不只是BIOS会出错，而且我们的程序也肯定会问题百出。另外，在内存的0xf0000号地址附近，还存放着BIOS程序本身，那里我们也不能使用。

##### 第一天

###### 涉及到的一些术语：

**FAT12格式（FAT12 Format）**

用Windows或MS-DOS格式化出来的软盘就是这种格式。我们的helloos也采用了这种格式，其中容纳了我们开发的操作系统。这个格式兼容性好，在Windows上也能用，而且剩余的磁盘空间还可以用来保存自己喜欢的文件。

**启动区（boot sector）**

软盘第一个的扇区称为启动区。那么什么是扇区呢？<u>计算机读写软盘的时候，并不是一个字节一个字节地读写的，而是以512字节为一个单位进行读写。因此,软盘的512字节就称为一个扇区。</u>一张软盘的空间共有1440KB，也就是1474560字节，除以512得2880，这也就是说一张软盘共有2880个扇区。那为什么第一个扇区称为启动区呢？那是因为计算机首先从最初一个扇区开始读软盘，然后去检查这个扇区最后2个字节的内容。如果这最后2个字节不是0x55 AA，计算机会认为这张盘上没有所需的启动程序，就会报一个不能启动的错误。（也许有人会问为什么一定是0x55 AA呢？那是当初的设计者随便定的，笔者也没法解释）。如果计算机确认了第一个扇区的最后两个字节正好是0x55 AA，那它就认为这个扇区的开头是启动程序，并开始执行这个程序。

###### IPL 

initial program loader的缩写。启动程序加载器。启动区只有区区512字节，实际的操作系统不像hello-os这么小，根本装不进去。所以几乎所有的操作系统，都是把加载操作系统本身的程序放在启动区里的。有鉴于此，有时也将启动区称为IPL。但hello-os没有加载程序的功能，所以HELLOIPL这个名字不太顺理成章。如果有人正义感特别强，觉得“这是撒谎造假，万万不能容忍！”，那也可以改成其他的名字。但是必须起一个8字节的名字，如果名字长度不到8字节的话，需要在最后补上空格。

###### 启动（boot）

boot这个词本是长靴（boots）的单数形式。它与计算机的启动有什么关系呢？一般应该将启动称为start的。实际上，boot这个词是bootstrap的缩写，原指靴子上附带的便于拿取的靴带。但自从有了《吹牛大王历险记》（德国）这个故事以后，bootstrap这个词就有了“自力更生完成任务”这种意思。而且，磁盘上明明装有操作系统，还要说读入操作系统的程序（即IPL）也放在磁盘里，这就像打开宝物箱的钥匙就在宝物箱里一样，是一种矛盾的说法。这种矛盾的操作系统自动启动机制，被称为bootstrap方式。boot这个说法就来源于此。如果是笔者来命名的话，肯定不会用bootstrap 这么奇怪的名字，笔者大概会叫它“多级火箭式”吧。

##### 第二天

**一些寄存器**：

AX——accumulator，累加寄存器

CX——counter，计数寄存器

DX——data，数据寄存器

BX——base，基址寄存器

SP——stack pointer，栈指针寄存器

BP——base pointer，基址指针寄存器

SI——source index，源变址寄存器

DI——destination index，目的变址寄存器

上面的寄存器就是 16 位的 

BP、SP、SI、DI没分为“L”和“H”。

[SI]可以用寄存器来制定内存地址，但可作此用途的寄存器非常有限，只有BX、BP、SI、DI这几个。剩下的AX、CX、DX、SP不能用来指定内存地址。所以想把DX内存里的内容赋值给AL的时候，就会这样写：

```assembly
mov BX,DX
mov AL,[BX]
```

###### BIOS

电脑里有个名为BIOS的程序，出厂时就组装在电脑主板上的ROM2单元里。电脑厂家在BIOS中预先写入了操作系统开发人员经常会用到的一些程序，非常方便。BIOS是英文“basic  input  output  system”的缩写，直译过来就是“基本输入输出系统（程序）”只读存储器，不能写入，切断电源以后内容不会消失。ROM是“read only memory”的缩写。

###### 内存分布图

https://wiki.osdev.org/index.php?title=Memory_Map_%28x86%29&oldid=13415

| start                         | end        | size                                          | type                                 | description                            |
| ----------------------------- | ---------- | --------------------------------------------- | ------------------------------------ | -------------------------------------- |
| Low Memory (the first MiB)    |            |                                               |                                      |                                        |
| 0x00000000                    | 0x000003FF | 1 KiB                                         | RAM - partially unusable (see above) | Real Mode IVT (Interrupt Vector Table) |
| 0x00000400                    | 0x000004FF | 256 bytes                                     | RAM - partially unusable (see above) | BDA (BIOS data area)                   |
| 0x00000500                    | 0x00007BFF | almost 30 KiB                                 | RAM (guaranteed free for use)        | Conventional memory                    |
| 0x00007C00 (typical location) | 0x00007DFF | 512 bytes                                     | RAM - partially unusable (see above) | Your OS BootSector                     |
| 0x00007E00                    | 0x0007FFFF | 480.5 KiB                                     | RAM (guaranteed free for use)        | Conventional memory                    |
| 0x00080000                    | 0x0009FBFF | approximately 120 KiB, depending on EBDA size | RAM (free for use, **if it exists**) | Conventional memory                    |
| 0x0009FC00 (typical location) | 0x0009FFFF | 1 KiB                                         | RAM (unusable)                       | EBDA (Extended BIOS Data Area)         |
| 0x000A0000                    | 0x000FFFFF | 384 KiB                                       | various (unusable)                   | Video memory, ROM Area                 |

##### 第三天

###### INT13

磁盘读、写，扇区校验（verify），以及寻道（seek）

```
AH=0x02;（读盘）
AH=0x03;（写盘）
AH=0x04;（校验）
AH=0x0c;（寻道）
AL=处理对象的扇区数;（只能同时处理连续的扇区）
CH=柱面号 &0xff;
CL=扇区号（0-5位）|（柱面号&0x300）* * 2;
DH=磁头号;
DL=驱动器号；
ES:BX=缓冲地址；(校验及寻道时不使用)
返回值：
FLACS.CF==0：没有错误，AH==0
FLAGS.CF==1：有错误，
错误号码存入AH内（与重置（reset）功能一样）
```

```
Cylinder = 0 to 1023 (maybe 4095), Head = 0 to 15 (maybe 254, maybe 255), Sector = 1 to 63

- Set AH = 2
- AL = total sector count (0 is illegal) -- cannot cross ES page boundary, **or a cylinder boundary**, and must be < 128
- CH = cylinder & 0xff
- CL = Sector | ((cylinder >> 2) & 0xC0);
- DH = Head -- may include two more cylinder bits
- ES:BX -> buffer
- Set DL = "drive number" -- typically 0x80, for the "C" drive
- Issue an INT 0x13.

The carry flag will be set if there is any error during the read. AH should be set to 0 on success.

To write: set AH to 3, instead.

Note: The limitation about not crossing cylinder boundaries is very annoying, especially when combined with the 127 sector limit -- because the arithmetic for the length and "start CHS" of the *next* consecutive read or write gets messy. The simplest workaround is to read or write only one sector at a time in CHS mode. Not all BIOSes have these two limitations, of course, but it is necessary to program for the "lowest common denominator".
```

###### 软盘

1张软盘有80个柱面，2个磁头，18个扇区，且一个扇区有512字节。所以，一张软盘的容量是：80×2×18×512 = 1 474 560 Byte = 1 440KB

含有IPL的启动区，位于C0-H0-S1（柱面0，磁头0，扇区1的缩写）.

下一个扇区是C0-H0-S2。这次我们想要装载的就是这个扇区。

C0-H0-S18 的下一扇区是 C0-H1-S1

