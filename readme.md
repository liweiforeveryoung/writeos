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

###### haribote.img

在0x002600附近，磁盘的这个位置好像保存着文件名

0x004200那里，可以看到“F4 EB FD”

F4 EB FD 即为 

```assembly
fin:
    HLT
    JMP     fin
```

所生成的二进制

###### 从启动区执行操作系统

要怎样才能执行磁盘映像上位于0x004200号地址的程序呢？现在的程序是从启动区开始，把磁盘上的内容装载到内存0x8000号地址，所以磁盘0x4200处的内容就应该位于内存0x8000+0x4200=0xc200号地址。这样的话，我们就往haribote.nas里加上ORG 0xc200，然后在ipl.nas处理的最后加上JMP0xc200这个指令。

> 其实是将磁盘的第2扇区一直到第 360 扇区的读取到了 0x8200~。可以视作将第1扇区读到了 0x8000，虽然并没有读取第一扇区的内容。

###### org 指令

汇编伪指令ORG作用是定义程序或数据块的起始地址，指示此语句后面的程序或数据块以nn为起始地址连续存放在程序存储器中。

ORG是Origin的缩写：起始地址，源。在汇编语言源程序的开始通常都用一条ORG伪指令来实现规定程序的起始地址。如果不用ORG规定则汇编得到的目标程序将从0000H开始。

ORG数值表达式伪指令的作用是：告诉汇编程序，本伪指令下面的内存变量从该“数值表达式”所指定的地址开始分配。汇编语言提供了一个特殊的符号“$”来引用偏移量计数器的值。

org 并不是说程序真的被加载到这个地址了，而是让程序自以为被加载到这个地址了。

###### INT10

```
设置显卡模式（video mode）

AH=0x00;

AL=模式：（省略了一些不重要的画面模式）

​	0x03：16色字符模式，80 × 25

​	0x12：VGA 图形模式，640 × 480 × 4位彩色模式，独特的4面存储模式

​	0x13：VGA 图形模式，320 × 200 × 8位彩色模式，调色板模式

​	0x6a：扩展VGA 图形模式，800 × 600 × 4位彩色模式，独特的4面存储模式（有的显卡不支持这个模式）

返回值：无
```

###### 32位模式前期准备

如果用32位模式就不能调用BIOS功能了。这是因为BIOS是用16位机器语言写的。<u>如果我们有什么事情想用BIOS来做，那就全部都放在开头先做</u>，因为一旦进入32位模式就不能调用BIOS函数了。（当然，也有从32位返回到16位的方法，但是非常费工夫，所以本书不予赘述。）再回头说说要使用BIOS做的事情。画面模式的设定已经做完了，接下来还想从BIOS得到键盘状态。所谓键盘状态，是指NumLock是ON还是OFF等这些状态。

###### INT16 AH=2

```
AH = 02h
Return:AL = shift flags (see #00582)
AH destroyed by many BIOSes
```

```
Bitfields for keyboard shift flags:

Bit(s)  Description     (Table 00582)
7      Insert active
6      CapsLock active
5      NumLock active
4      ScrollLock active
3      Alt key pressed (either Alt on 101/102-key keyboards)
2      Ctrl key pressed (either Ctrl on 101/102-key keyboards)
1      left shift key pressed
0      right shift key pressed
```

###### 导入 c 语言

这个bootpack.c是怎样变成机器语言的呢？如果不能变成机器语言，就是说得再多也没有意义。这个步骤很长，让我们看一看。

```
首先，使用cc1.exe从bootpack.c生成bootpack.gas。

第二步，使用gas2nask.exe从bootpack.gas生成bootpack.nas。

第三步，使用nask.exe从bootpack.nas生成bootpack.obj。

第四步，使用obi2bim.exe从bootpack.obj生成bootpack.bim。

最后，使用bim2hrb.exe从bootpack.bim生成bootpack.hrb。这样就做成了机器语言，再使用copy指令将asmhead.bin与bootpack.hrb单纯结合到起来，就成了haribote.sys。
```

##### 第四天

###### instrset

它是用来告诉nask“这个程序是给486用的哦”，nask见了这一行之后就知道“哦，那见了EAX这个词，就解释成寄存器名”。如果什么都不指定，它就会认为那是为8086这种非常古老的、而且只有16位寄存器的CPU而写的程序，见了EAX这个词，会误解成标签（Label），或是常数。8086那时候写的程序中，曾偶尔使用EAX来做标签，当时也没想到这个单词后来会成为寄存器名而不能再随便使用。

###### 色号设定

这次使用的是320×   200的8位颜色模式，色号使用8位（二进制）数，也就是只能使用0～255的数。

仅使用这几个颜色

```
#000000:黑
#00ffff:浅亮蓝
#000084:暗蓝
#ff0000:亮红
#ffffff:白
#840084:暗紫
#00ff00:亮绿
#c6c6c6:亮灰
#008484:浅暗蓝
#ffff00:亮黄
#840000:暗红
#848484:暗灰
#0000ff:亮蓝
#008400:暗绿
#ff00ff:亮紫
#848400:暗黄
```

###### IN/OUT

CPU的管脚与内存相连。如果仅仅是与内存相连，CPU就只能完成计算和存储的功能。但实际上，CPU还要对键盘的输入有响应，要通过网卡从网络取得信息，通过声卡发送音乐数据，向软盘写入信息等。这些都是设备（device），它们当然也都要连接到CPU上。既然CPU与设备相连，那么就有向这些设备发送电信号，或者从这些设备取得信息的指令。向设备发送电信号的是OUT指令；从设备取得电气信号的是IN指令。正如为了区别不同的内存要使用内存地址一样，在OUT指令和IN指令中，为了区别不同的设备，也要使用设备号码。设备号码在英文中称为port（端口）。port原意为“港口”，这里形象地将CPU与各个设备交换电信号的行为比作了船舶的出港和进港。

###### 设置调色板

调色板的访问步骤。首先在一连串的访问中屏蔽中断（比如CLI）。将想要设定的调色板号码写入0x03c8，紧接着，按R，G，B的顺序写入0x03c9。如果还想继续设定下一个调色板，则省略调色板号码，再按照RGB的顺序写入0x03c9就行了。如果想要读出当前调色板的状态，首先要将调色板的号码写入0x03c7，再从0x03c9读取3次。读出的顺序就是R，G，B。如果要继续读出下一个调色板，同样也是省略调色板号码的设定，按RGB的顺序读出。如果最初执行了CLI，那么最后要执行STI

###### CLI/STI

首先是CLI和STI。所谓CLI，是将中断标志（interrupt  flag）置为0的指令（clear  interrupt  flag）。STI是要将这个中断标志置为1的指令（setinterrupt   flag）。

更改中断标志有什么好处呢？正如其名所示，它与CPU的中断处理有关系。当CPU遇到中断请求时，是立即处理中断请求（中断标志为1），还是忽略中断请求（中断标志为0），就由这个中断标志位来设定。

###### EFLAGS

下面再来介绍一下EFLAGS这一特别的寄存器。这是由名为FLAGS的16位寄存器扩展而来的32位寄存器。FLAGS是存储进位标志和中断标志等标志的寄存器。进位标志可以通过JC或JNC等跳转指令来简单地判断到

底是0还是1。但对于中断标志，没有类似的JI或JNI命令，所以只能读入EFLAGS，再检查第9位是0还是1。顺便说一下，进位标志是EFLAGS的第0位。空白位没有特殊意义（或许留给将来的CPU用？）set_palette中想要做的事情是在设定调色板之前首先执行CLI，但处理结束以后一定要恢复中断标志，因此需要记住最开始的中断标志是什么。所以我们制作了一个函数io_load_eflags，读取最初的eflags值。处理结束以后，可以先看看eflags的内容，再决定是否执行STI，但仔细想一想，也没必要搞得那么复杂，干脆将eflags的值代入EFLAGS，中断标志位就恢复为原来的值了。函数o_store_eflags就是完成这个处理的。估计不说大家也知道了，CLI也好，STI也好，EFLAGS的读取也好，EFLAGS的写入也好，都不能用C语言来完成。所以我们就努力一下，用汇编语言来写吧。

##### 第五天

###### char 和 unsigned char 进行位运算的一个坑

```c
int main() {
    char flag = (char) 0x80;
    for (int i = 0; i < 8; ++i) {
        printf("%x\n", flag);
        flag >>= 1;
    }
    printf("============\n");
    unsigned char uFlag = 0x80;
    for (int i = 0; i < 8; ++i) {
        printf("%x\n", uFlag);
        uFlag >>= 1;
    }
}
```

输出：

```
ffffff80
ffffffc0
ffffffe0
fffffff0
fffffff8
fffffffc
fffffffe
ffffffff
============
80
40
20
10
8
4
2
1
```

所以，进行位运算时用 unsigned char。

###### GDT与IDT的初始化

GDT也好，IDT也好，它们都是与CPU有关的设定。为了让操作系统能够使用32位模式，需要对CPU做各种设定。

###### 分段

可以将内存分成多个段，每个段可以视作一段独立的内存空间。代码的地址是相对于该段的地址而言。

> 需要注意的一点是，我们用16位的时候曾经讲解过的段寄存器。这里的分段，使用的就是这个段寄存器。但是16位的时候，如果计算地址，只要将地址乘以16就可以了。但现在已经是32位了，不能再这么用了。如果写成“MOV    AL,[DS:EBX]”，CPU会往EBX里加上某个值来计算地址，这个值不是DS的16倍，而是DS所表示的段的起始地址。即使省略段寄存器（segment  register）的地址，也会自动认为是指定了DS。这个规则不管是16位模式还是32位模式，都是一样的。

段的信息

按这种分段方法，为了表示一个段，需要有以下信息。

+ 段的大小是多少

+ 段的起始地址在哪里

+ 段的管理属性（禁止写入，禁止执行，系统专用等）

CPU用8个字节（=64位）的数据来表示这些信息。但是，用于指定段的寄存器只有16位。或许有人会猜想在32位模式下，段寄存器会扩展到64位，但事实上段寄存器仍然是16位。

###### GDT

在调色板后，我们可以设置色号表，在表中记录每一个色号所代表的颜色。但是色号只能是 0~255的数。

同理，也有一个段号表，段号可以用0~8191（6 * 1024）的数。因为段寄存器是16位，所以本来应该能够处理0~65535范围的数，但由于CPU设计上的原因，段寄存器的低3位不能使用。因此能够使用的段号只有13位，能够处理的就只有位于0~8191的区域了。

但因为能够使用0~8191的范围，即可以定义8192个段，所以设定这么多段就需要8192×8=65  536字节（64KB）。

###### GDT （global（segment）descriptor  table）

GDT是“global（segment）descriptor  table”的缩写，意思是全局段号记录表。将这些数据整齐地排列在内存的某个地方，然后将内存的起始地址和有效设定个数放在CPU内被称作GDTR5的特殊寄存器中，设定就完成了。

###### IDT （interrupt descriptor table）

另外，IDT是“interrupt descriptor table”的缩写，直译过来就是“中断记录表”。当CPU遇到外部状况变化，或者是内部偶然发生某些错误时，会临时切换过去处理这种突发事件。这就是中断功能。

要使用鼠标，就必须要使用中断。所以，我们必须设定IDT。IDT记录了0～255的中断号码与调用函数的对应关系，比如说发生了123号中断，就调用○×函数，其设定方法与GDT很相似（或许是因为使用同样的方法能简化CPU的电路）。

如果段的设定还没顺利完成就设定IDT的话，会比较麻烦，所以必须先进行GDT的设定。

##### 第六天

###### load_gdtr 和 load_idtr 的解读

```assembly
_load_gdtr: ; void load_gdtr(int limit,int addr)    segment descriptor
    MOV     AX,[ESP+4]      ; limit
    MOV     [ESP+6],AX      ; todo ???
    LGDT    [ESP+6]
    RET


_load_idtr: ; void load_idtr(int limit,int addr)    interrupt descriptor
    MOV		AX,[ESP+4]		; limit
    MOV		[ESP+6],AX      ; todo ???
    LIDT	[ESP+6]
    RET
```

先看 load_gdtr。这个函数用来将指定的段上限（limit）和地址值赋值给名为GDTR的48位寄存器。这是一个很特别的48位寄存器，并不能用我们常用的MOV指令来赋值。给它赋值的时候，唯一的方法就是指定一个内存地址，从指定的地址读取6个字节（也就是48位），然后赋值给GDTR寄存器。完成这一任务的指令，就是LGDT。

该寄存器的低16位1（即内存的最初2个字节）是段上限，它等于“GDT的有效字节数  -  1”。今后我们还会偶尔用到上限这个词，意思都是表示量的大小，一般为“字节数  -  1”。剩下的高32位（即剩余的4个字节），代表GDT的开始地址。

![image-20210330221256472](readme.assets/image-20210330221256472.png)

```drawio
<mxfile>
    <diagram id="cE5uHtphANISIs85Q7SA" name="第 1 页">
        <mxGraphModel dx="848" dy="611" grid="1" gridSize="10" guides="1" tooltips="1" connect="1" arrows="1" fold="1" page="1" pageScale="1" pageWidth="3300" pageHeight="4681" math="0" shadow="0">
            <root>
                <mxCell id="0"/>
                <mxCell id="1" parent="0"/>
                <mxCell id="32" value="" style="rounded=0;whiteSpace=wrap;html=1;" parent="1" vertex="1">
                    <mxGeometry x="280" y="440" width="80" height="480" as="geometry"/>
                </mxCell>
                <mxCell id="33" value="" style="endArrow=classic;html=1;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="120" y="440" as="sourcePoint"/>
                        <mxPoint x="120" y="920" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="34" value="低地址" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="60" y="440" width="50" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="35" value="高地址" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="60" y="900" width="50" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="40" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" source="32" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="440" y="520" as="sourcePoint"/>
                        <mxPoint x="360" y="480" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="41" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="520" as="sourcePoint"/>
                        <mxPoint x="360" y="520.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="42" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="280" y="560" as="sourcePoint"/>
                        <mxPoint x="360.15999999999997" y="560.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="43" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="600" as="sourcePoint"/>
                        <mxPoint x="360" y="600.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="44" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="640" as="sourcePoint"/>
                        <mxPoint x="360" y="640.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="45" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="280" y="679.5" as="sourcePoint"/>
                        <mxPoint x="360.15999999999997" y="679.66" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="46" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="720" as="sourcePoint"/>
                        <mxPoint x="360" y="720.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="47" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="760" as="sourcePoint"/>
                        <mxPoint x="360" y="760.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="48" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="280" y="800" as="sourcePoint"/>
                        <mxPoint x="360.15999999999997" y="800.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="49" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84" y="840" as="sourcePoint"/>
                        <mxPoint x="359.99999999999994" y="840.16" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="50" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" parent="1" edge="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="279.84000000000003" y="880" as="sourcePoint"/>
                        <mxPoint x="360" y="880.1600000000001" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="51" value="esp + 0" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="450" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="53" value="esp + 1" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="490" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="54" value="esp + 2" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="530" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="55" value="esp + 3" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="570" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="56" value="esp + 4" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="610" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="57" value="esp + 5" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="650" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="58" value="esp + 6" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="690" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="59" value="esp + 7" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="730" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="60" value="esp + 8" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="770" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="61" value="esp + 9" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="290" y="810" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="62" value="esp + 10" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="285" y="850" width="70" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="63" value="esp + 11" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" parent="1" vertex="1">
                    <mxGeometry x="285" y="890" width="70" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="67" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" parent="1" vertex="1">
                    <mxGeometry x="250" y="440" width="20" height="160" as="geometry"/>
                </mxCell>
                <mxCell id="68" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" parent="1" vertex="1">
                    <mxGeometry x="250" y="600" width="20" height="160" as="geometry"/>
                </mxCell>
                <mxCell id="69" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" parent="1" vertex="1">
                    <mxGeometry x="250" y="760" width="20" height="160" as="geometry"/>
                </mxCell>
                <mxCell id="70" value="cs + ip" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="190" y="510" width="50" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="71" value="limit" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="190" y="670" width="40" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="72" value="addr" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="190" y="830" width="40" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="73" value="" style="shape=singleArrow;whiteSpace=wrap;html=1;" vertex="1" parent="1">
                    <mxGeometry x="430" y="640" width="100" height="60" as="geometry"/>
                </mxCell>
                <mxCell id="74" value="" style="rounded=0;whiteSpace=wrap;html=1;" vertex="1" parent="1">
                    <mxGeometry x="650" y="440" width="80" height="480" as="geometry"/>
                </mxCell>
                <mxCell id="75" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1" source="74">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="810" y="520" as="sourcePoint"/>
                        <mxPoint x="730" y="480" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="76" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="520" as="sourcePoint"/>
                        <mxPoint x="730" y="520.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="77" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="650" y="560" as="sourcePoint"/>
                        <mxPoint x="730.1599999999999" y="560.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="78" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="600" as="sourcePoint"/>
                        <mxPoint x="730" y="600.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="79" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="640" as="sourcePoint"/>
                        <mxPoint x="730" y="640.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="80" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="650" y="679.5" as="sourcePoint"/>
                        <mxPoint x="730.1599999999999" y="679.6599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="81" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="720" as="sourcePoint"/>
                        <mxPoint x="730" y="720.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="82" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="760" as="sourcePoint"/>
                        <mxPoint x="730" y="760.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="83" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="650" y="800" as="sourcePoint"/>
                        <mxPoint x="730.1599999999999" y="800.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="84" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.8399999999999" y="840" as="sourcePoint"/>
                        <mxPoint x="730" y="840.1599999999999" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="85" value="" style="endArrow=none;html=1;exitX=-0.002;exitY=0.083;exitDx=0;exitDy=0;exitPerimeter=0;" edge="1" parent="1">
                    <mxGeometry width="50" height="50" relative="1" as="geometry">
                        <mxPoint x="649.84" y="880" as="sourcePoint"/>
                        <mxPoint x="730" y="880.1600000000001" as="targetPoint"/>
                    </mxGeometry>
                </mxCell>
                <mxCell id="86" value="esp + 0" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="450" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="87" value="esp + 1" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="490" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="88" value="esp + 2" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="530" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="89" value="esp + 3" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="570" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="104" style="edgeStyle=orthogonalEdgeStyle;rounded=0;orthogonalLoop=1;jettySize=auto;html=1;entryX=1.008;entryY=0.549;entryDx=0;entryDy=0;entryPerimeter=0;fillColor=#60a917;strokeColor=#2D7600;" edge="1" parent="1" target="74">
                    <mxGeometry relative="1" as="geometry">
                        <mxPoint x="731" y="620" as="sourcePoint"/>
                        <Array as="points">
                            <mxPoint x="750" y="620"/>
                            <mxPoint x="750" y="704"/>
                        </Array>
                    </mxGeometry>
                </mxCell>
                <mxCell id="90" value="esp + 4" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="610" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="106" style="edgeStyle=orthogonalEdgeStyle;rounded=0;orthogonalLoop=1;jettySize=auto;html=1;entryX=0.988;entryY=0.625;entryDx=0;entryDy=0;entryPerimeter=0;fillColor=#6a00ff;strokeColor=#3700CC;" edge="1" parent="1" target="74">
                    <mxGeometry relative="1" as="geometry">
                        <mxPoint x="729" y="660" as="sourcePoint"/>
                        <Array as="points">
                            <mxPoint x="750" y="660"/>
                            <mxPoint x="750" y="740"/>
                        </Array>
                    </mxGeometry>
                </mxCell>
                <mxCell id="91" value="esp + 5" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="650" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="92" value="esp + 6" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="690" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="93" value="esp + 7" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="730" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="94" value="esp + 8" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="770" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="95" value="esp + 9" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="660" y="810" width="60" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="96" value="esp + 10" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="655" y="850" width="70" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="97" value="esp + 11" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="655" y="890" width="70" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="98" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" vertex="1" parent="1">
                    <mxGeometry x="620" y="440" width="20" height="160" as="geometry"/>
                </mxCell>
                <mxCell id="99" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" vertex="1" parent="1">
                    <mxGeometry x="620" y="680" width="20" height="80" as="geometry"/>
                </mxCell>
                <mxCell id="100" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;" vertex="1" parent="1">
                    <mxGeometry x="620" y="760" width="20" height="160" as="geometry"/>
                </mxCell>
                <mxCell id="101" value="cs + ip" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="570" y="510" width="50" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="102" value="limit" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="580" y="710" width="40" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="103" value="addr" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="580" y="830" width="40" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="107" value="AX" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="615" y="630" width="30" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="108" value="AX" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="750" y="710" width="30" height="20" as="geometry"/>
                </mxCell>
                <mxCell id="109" value="" style="shape=curlyBracket;whiteSpace=wrap;html=1;rounded=1;rotation=-180;" vertex="1" parent="1">
                    <mxGeometry x="800" y="680" width="20" height="240" as="geometry"/>
                </mxCell>
                <mxCell id="110" value="调用 LGDT" style="text;html=1;align=center;verticalAlign=middle;resizable=0;points=[];autosize=1;" vertex="1" parent="1">
                    <mxGeometry x="820" y="790" width="70" height="20" as="geometry"/>
                </mxCell>
            </root>
        </mxGraphModel>
    </diagram>
</mxfile>
```

###### SEGMENT_DESCRIPTOR的解读

```
struct SEGMENT_DESCRIPTOR {
    short limit_low;
    short base_low;     // 段的地址
    char base_mid;      // 段的地址
    char access_right;
    char limit_high;
    char base_high;     // 段的地址
};
```

首先看一下段的地址。地址当然是用32位来表示。这个地址在CPU世界的语言里，被称为段的基址。所以这里使用了base这样一个变量名。在这个结构体里base又分为low（2字节），mid（1字节），high（1字节）3段，合起来刚好是32位。所以，这里只要按顺序分别填入相应的数值就行了。虽然有点难懂，但原理很简单。程序中使用了移位运算符和AND运算符往各个字节里填入相应的数值。为什么要分为3段呢？主要是为了与80286时代的程序兼容。有了这样的规格，80286用的操作系统，也可以不用修改就在386以后的CPU上运行了。

下面再说一下段上限。它表示一个段有多少个字节。可是这里有一个问题，段上限最大是4GB，也就是一个32位的数值，如果直接放进去，这个数值本身就要占用4个字节，再加上基址（base），一共就要8个字节，这就把整个结构体占满了。这样一来，就没有地方保存段的管理属性信息了，这可不行。因此段上限只能使用20位。这样一来，段上限最大也只能指定到1MB为止。明明有4GB，却只能用其中的1MB，有种又回到了16位时代的错觉，太可悲了。在这里英特尔的叔叔们又想了一个办法，他们在段的属性里设了一个标志位，叫做Gbit。这个标志位是1的时候，limit的单位不解释成字节（byte），而解释成页（page）。页是什么呢？在电脑的CPU里，1页是指4KB。这样一来，4KB × 1M = 4GB，所以可以指定4GB的段。总算能放心了。顺便说一句，G bit的“G”，是“granularity”的缩写，是指单位的大小。这20位的段上限分别写到limit_low和limit_high里。看起来它们好像是总共有3字节，即24位，但实际上我们接着要把段属性写入limit_high的高4位里，所以最后段上限还是只有20，好复杂呀

最后再来讲一下12位的段属性。段属性又称为“段的访问权属性”，在程序中用变量名access_right或ar来表示。因为12位段属性中的高4位放在limit_high的高4位里，所以程序里有意把ar当作如下的16位构成来处理。

xxxx0000xxxxxxxx(其中x是0或1)

ar的高4位被称为“扩展访问权”。为什么这么说呢？因为这高4位的访问属性在80286的时代还不存在，到386以后才可以使用。这4位是由“GD00”构成的，其中G是指刚才所说的Gbit，D是指段的模式，1是指32位模式，0是指16位模式。这里出现的16位模式主要只用于运行80286的程序，不能用于调用BIOS。所以，除了运行80286程序以外，通常都使用D=1的模式。

ar的低8位从80286时代就已经有了，如果要详细说明的话，够我们说一天的了，所以这里只是简单地介绍一下。00000000（0x00）：未使用的记录表（descriptor table）。

```
10010010（0x92）：系统专用，可读写的段。不可执行
10011010（0x9a）：系统专用，可执行的段。可读不可写
11110010（0xf2）：应用程序用，可读写的段。不可执行
11111010（0xfa）：应用程序用，可执行的段。可读不可写
```

###### PIC （programmable  interrupt  controller）

所谓PIC是“programmable  interrupt  controller”的缩写，意思是“可编程中断控制器”。PIC与中断的关系可是很密切的哟。它到底是什么呢？在设计上，CPU单独只能处理一个中断，这不够用，所以IBM的大叔们在设计电脑时，就在主板上增设了几个辅助芯片。现如今它们已经被集成在一个芯片组里了

PIC是将8个中断信号（interrupt request，缩写为IRQ）集合成一个中断信号的装置。PIC监视着输入管脚的8个中断信号，只要有一个中断信号进来，就将唯一的输出管脚信号变成ON，并通知给CPU。IBM的大叔们想要通过增加PIC来处理更多的中断信号，他们认为电脑会有8个以上的外部设备，所以就把中断信号设计成了15个，并为此增设了2个PIC。

与CPU直接相连的PIC称为主PIC（master  PIC），与主PIC相连的PIC称为从PIC（slave  PIC）。主PIC负责处理第0到第7号中断信号，从PIC负责处理第8到第15号中断信号

![image-20210401203152955](readme.assets/image-20210401203152955.png)

程序中的PIC0和PIC1，分别指主PIC和从PIC。

###### PIC 的寄存器

PIC的寄存器都是八位的。

1. IMR（interrupt  mask  register）终端屏蔽寄存器

   8位分别对应8路IRQ信号。如果某一位的值是1，则该位所对应的IRQ信号被屏蔽，PIC就忽视该路信号。

2. ICW （initial  control  word）初始化控制数据

   ICW有4个，分别编号为1~4，

   + ICW1：与PIC主板配线方式、中断信号的电气特性等有关
   + ICW2：不同的操作系统可以进行独特设定的就只有ICW2了。这个ICW2，决定了IRQ以哪一号中断通知CPU。
   + ICW3：ICW3是有关主—从连接的设定，
     + 对主PIC而言，第几号IRQ与从PIC相连，是用8位来设定的。如果把这些位全部设为1，那么主PIC就能驱动8个从PIC（那样的话，最大就可能有64个IRQ）。我们用的2号，所以设置为 00000100（前面还有0号和1号）。
     + 从PIC来说，该从PIC与主PIC的第几号相连，用3位来设定。
   + ICW4：与PIC主板配线方式、中断信号的电气特性等有关

###### ICW2 大骗子

下。中断发生以后，如果CPU可以受理这个中断，CPU就会命令PIC发送2个字节的数据。这2个字节是怎么传送的呢？CPU与PIC用IN或OUT进行数据传送时，有数据信号线连在一起。PIC就是利用这个信号线发送这2个字节数据的。送过来的数据是“0xcd  0x??”这两个字节。由于电路设计的原因，这两个字节的数据在CPU看来，与从内存读进来的程序是完全一样的，所以CPU就把送过来的“0xcd   0x??”作为机器语言执行。这恰恰就是把数据当作程序来执行的情况。这里的0xcd就是调用BIOS时使用的那个INT指令。我们在程序里写的“INT  0x10”，最后就被编译成了“0xcd 0x10”。所以，CPU上了PIC的当，按照PIC所希望的中断号执行了INT指令。

以INT 0x20~0x2f接收中断信号IRQ0~15

INT 0x00~0x1f不能用于IRQ，因为应用程序想要对操作系统干坏事的时候，CPU内部会自动产生INT  0x00~0x1f，如果IRQ与这些号码重复了，  CPU就分不清它到底是IRQ，还是CPU的系统保护通知。

###### PUSHAD 和 POPAD

pushad 相当于

```assembly
PUSH EAX
PUSH ECX
PUSH EDX
PUSH EBX
PUSH ESP
PUSH EBP
PUSH ESI
PUSH EDI
```

POPAD指令相当于按以上相反的顺序，把它们全都POP出来。

###### sti 和 cli

执行STI指令后，IF（interrupt   flag，中断许可标志位）变为1，CPU接受来自外部设备的中断（参考4.6节）。CPU的中断信号只有一根，所以IF也只有一个，不像PIC那样有8位。

##### 第七天

######  io_out8(PIC0_OCW2, 0x6

这句话用来通知PIC“已经知道发生了IRQ1中断哦”。如果是IRQ3，则写成0x63。也就是说，将“0x60+IRQ号码”输出给OCW2就可以。执行这句话之后，PIC继续时刻监视IRQ1中断是否发生。反过来，如果忘记了执行这句话，PIC就不再监视IRQ1中断，不管下次由键盘输入什么信息，系统都感知不到了。

> 意思是如果你不执行这句话，那么 cpu 就认为你还没有处理完 irq1，即使下次 irq1 再次发生了，cpu 也不会告诉你了，哼。

> 更新：试验后发现，当 irq No 发生时，如果不告诉 pic 受理完成，之后所有的 irq 事件，pic 都不会通知你。

######  io_stihlt()

这是作者原本写的代码

```c
for (;;) {
	io_cli();
	if (keybuf.flag == 0) {
		io_stihlt();
	} else {
		i = keybuf.data;
		keybuf.flag = 0;
		io_sti();
		sprintf(s, "%02X", i);
		boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
	}
}
```

作者用上了 io_stihlt()，并对原因做出了解释，解释如下：

> 可能有人会认为，不做这个函数，而是用“io_sti();io_hlt();”不也行吗？但是，实际上这样写有点问题。如果io_sti()之后产生了中断，keybuf里就会存入数据，这时候让CPU进入HLT状态，keybuf里存入的数据就不会被觉察到。根据CPU的规范，机器语言的STI指令之后，如果紧跟着HLT指令，那么就暂不受理这两条指令之间的中断，而要等到HLT指令之后才受理，所以使用io_stihlt函数就能克服这一问题。

###### 鼠标

要想让鼠标正常工作，必须让下面两个装置有效，一个是鼠标控制电路，一个是鼠标本身。

鼠标控制电路包含在键盘控制电路里，如果键盘控制电路的初始化正常完成，鼠标电路控制器的激活也就完成了。

wait_KBC_sendready。它的作用是，让键盘控制电路（keyboard  controller,  KBC）做好准备动作，等待控制指令的到来。为什么要做这个工作呢？是因为虽然CPU的电路很快，但键盘控制电路却没有那么快。如果CPU不顾设备接收数据的能力，只是一个劲儿地发指令的话，有些指令会得不到执行，从而导致错误的结果。如果键盘控制电路可以接受CPU指令了。

所以每次要对键盘控制电路发送命令前，多需要检查一下键盘控制电路的状态，要等它准备好后，才给它发送下一个指令。

###### 从鼠标接收数据

IRQ-12是从PIC的第4号（从PIC相当于IRQ-08～IRQ-15），首先要通知IRQ-12受理已完成，然后再通知主PIC。这是因为主/从PIC的协调不能够自动完成，如果程序不教给主PIC该怎么做，它就会忽视从PIC的下一个中断请求。从PIC连接到主PIC的第2号上。

##### 第八天

###### 解读鼠标数据

首先要把最初读到的0xfa舍弃掉。之后，每次从鼠标那里送过来的数据都应该是3个字节一组的，所以每当数据累积到3个字节，就把它显示在屏幕上。

幕上会出现类似于“08  12  34”之类的3字节数字。如果移动鼠标，

这个“08”部分（也就是mouse_dbuf[0]）的“0”那一位，会在0～3的范围内变化。另外，如果只是移动鼠标，08部分的“8”那一位，不会有任何变化，只有当点击鼠标的时候它才会变化。不仅左击有反应，右击和点击中间滚轮时都会有反应。不管怎样点击鼠标，这个值会在8～F之间变化。

> 试验后发现：
>
> 正常状态是：?8		8 = 1000
>
> 点鼠标左键是：?9	9 = 1001
>
> 点鼠标右键是：?a	a = 1010
>
> 点鼠标滚轮是：?c    c = 1100
>
> 滑动滚轮是：无效果
>
> 所有只要 & 0111，就可以知道按的是哪个键了

上述“12”部分（mouse_dbuf[1]）与鼠标的左右移动有关系。**它的值并不是鼠标真正的 x 坐标，而是鼠标当前位置相对于上一时刻位置的唯一矢量。**

“34”部分（mouse_dbuf[2]）则与鼠标的上下移动有关系。**它的值并不是鼠标真正的 x 坐标，而是鼠标当前位置相对于上一时刻位置的唯一矢量。**

###### A20GATE

这次输出0xdf所要完成的功能，是让A20GATE信号线变成ON的状态。这条信号线的作用是什么呢？它能使内存的1MB以上的部分变成可使用状态。最初出现电脑的时候，CPU只有16位模式，所以内存最大也只有1MB。后来CPU变聪明了，可以使用很大的内存了。但为了兼容旧版的操作系统，在执行激活指令之前，电路被限制为只能使用1MB内存。和鼠标的情况很类似哟。A20GATE信号线正是用来使这个电路停止从而让所有内存都可以使用的东西。

###### 保护模式

保护模式与先前的16位模式不同，段寄存器的解释不是16倍，而是能够使用GDT。这里的“保护”，来自英文的“protect”。在这种模式下，应用程序既不能随便改变段的设定，又不能使用操作系统专用的段。操作系统受到CPU的保护，所以称为保护模式。

> 本来的说法应该是“protected  virtual  address  mode”，翻译过来就是“受保护的虚拟内存地址模式”。与此相对，从前的16位模式称为“real  mode”，它是“real  address  mode”的省略形式，翻译过来就是“实际地址模式”。这些术语中的“virtual”，“real”的区别在于计算内存地址时，是使用段寄存器的值直接指定地址值的一部分呢，还是通过GDT使用段寄存器的值指定并非实际存在的地址号码。

在保护模式中，有带保护的16位模式，和带保护的32位模式两种。我们要使用的，是带保护的32位模式。讲解CPU的书上会写到，通过代入CR0而切换到保护模式时，要马上执行JMP指令。所以我们也执行这一指令。为什么要执行JMP指令呢？因为变成保护模式后，机器语言的解释要发生变化。CPU为了加快指令的执行速度而使用了管道（pipeline）这一机制，就是说，前一条指令还在执行的时候，就开始解释下一条甚至是再下一条指令。因为模式变了，就要重新解释一遍，所以加入了JMP指令。

而且在程序中，进入保护模式以后，段寄存器的意思也变了（不再是乘以16后再加算的意思了），除了CS以外所有段寄存器的值都从0x0000变成了0x0008。CS保持原状是因为如果CS也变了，会造成混乱，所以只有CS要放到后面再处理。0x0008，相当于“gdt + 1”的段。

###### 内存分布图

```
0x00000000  -  0x000fffff  : 虽然在启动中会多次使用，但之后就变空。（1MB）
0x00100000 - 0x00267fff : 用于保存软盘的内容。（1440KB）0x00268000 - 0x0026f7ff : 空（30KB）
0x0026f800 - 0x0026ffff : IDT（2KB）
0x00270000 - 0x0027ffff : GDT（64KB）
0x00280000 - 0x002fffff : bootpack.hrb（512KB）
0x00300000 - 0x003fffff : 栈及其他（1MB）
0x00400000 - : 空
```

##### 第九天

###### 内存容量检查

第一次失败：

原始代码：

```c
bool memory_is_valid(unsigned int *pMemory) {
    unsigned int pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    *pMemory = pat0;
    *pMemory ^= 0xffffffff;
    if (*pMemory != pat1) {
        return false;
    }
    *pMemory ^= 0xffffffff;
    if (*pMemory != pat0) {
        return false;
    }
    return true;
}
```

生成的汇编代码：

```assembly
PUSH	EBP
MOV	EBP,ESP
MOV	EAX,DWORD [8+EBP]
MOV	DWORD [EAX],-1437226411
MOV	EAX,1
POP	EBP
RET
```

-1437226411 即 0xAA55AA55。

可以看出，编译器将 `memory_is_valid` 优化成了这个样子：

```c
bool memory_is_valid(unsigned int *pMemory) {
    *pMemory = 0xaa55aa55;
    return true;
}
```

###### 自己亲手写汇编

自己写的汇编 ~

```assembly
_asm_memory_is_valid:   ; bool asm_memory_is_valid(unsigned int *pMemory)
        PUSH	EBP
        MOV	EBP,ESP
        MOV	EAX,DWORD [8+EBP]

        PUSH ECX

        MOV	DWORD [EAX],0xaa55aa55  ; *pMemory = 0xaa55aa55
        MOV	ECX,[EAX]               ;  temp = *pMemory
        XOR ECX,0xffffffff          ;  temp ^= 0xffffffff
        MOV [EAX],ECX               ; *pMemory = temp
        MOV	ECX,[EAX]               ;  temp = *pMemory
        CMP ECX,0x55aa55aa          ; if (temp != 0x55aa55aa)
        JNE not_valid               ;       return 0
        XOR ECX,0xffffffff          ;  temp ^= 0xffffffff
        MOV [EAX],ECX               ;  *pMemory = temp
        MOV ECX,[EAX]               ;  temp = *pMemory
        CMP ECX,0xaa55aa55          ; if (temp != 0xaa55aa55)
        JNE not_valid               ;      return 0
        MOV EAX,1                   ;  return 1
        JMP return
   not_valid:
        MOV EAX,0
   return:
        POP ECX
        POP	EBP
        RET
```

现在得到的结果如愿为 32MB 了。

##### 第十二天 定时器

哦，咋突然从第九天跳到第第二天了。。

定时器1（Timer）对于操作系统非常重要。它在原理上却很简单，只是每隔一段时间（比如0.01秒）就发送一个中断信号给CPU。幸亏有了定时器，CPU才不用辛苦地去计量时间。

###### PIT  （Programmable  Interval  Timer）

（和第六天的 PIC 长得很像）

要在电脑中管理定时器，只需对PIT进行设定就可以了。PIT是“Programmable  Interval  Timer”的缩写，翻译过来就是“可编程的间隔型定时器”。我们可以通过设定PIT，让定时器每隔多少秒就产生一次中断。因为在电脑中PIT连接着IRQ（interrupt  request，参考第6章）的0号，所以只要设定了PIT就可以设定IRQ0的中断间隔。

在旧机种上PIT是作为一个独立的芯片安装在主板上的，而现在已经和PIC（programmable  interrupt  controller，参考第6章）一样被集成到别的芯片里了。

电脑里的定时器用的是8254芯片（或其替代品），它的主频是 119.318khz。

```
RQ0的中断周期变更:

AL=0x34:OUT(0x43,AL);

AL=中断周期的低8位; OUT(0x40,AL);

AL=中断周期的高8位; OUT(0x40,AL);

如果指定中断周期为0，会被看作是指定为65536。实际的中断产生的频率是单位时间时钟周期数（即主频）/设定的数值。比如设定值如果是1000，那么中断产生的频率就是1.19318KHz。设定值是10000的话，中断产生频率就是119.318Hz。再比如设定值是11932的话，中断产生的频率大约就是100Hz了，即每10ms发生一次中断。
```

我们不清楚其中的详细原理，只知道只要执行3次OUT指令设定就完成了。将中断周期设定为11932的话，中断频率好像就是100Hz，也就是说1秒钟会发生100次中断。那么我们就设定成这个值吧。把11932换算成十六进制数就是0x2e9c。

##### 第十四天 提高分辨率

###### VBE

多家显卡公司经过协商，成立了VESA协会（VideoElectronics  Standards  Association，视频电子标准协会）。此后，这个协会制定了虽然不能说完全兼容、但几乎可以通用的设定方法，制作了专用的BIOS。这个追加的BIOS被称作“VESA  BIOS  extension”（VESA-BIOS扩展，简略为**VBE**）。

利用它，就可以使用显卡的高分辨率功能了。因此，切换到不使用VBE的画面模式时用

```asm
AH=0;
AL=画面模式号码
```

而切换到使用VBE的画面模式时用。

```assembly
AX = 0x4f02;
BX = 画面模式号码;
```

而这种必须使用VBE才能利用的画面模式就称作“新”画面模式。

VBE的画面模式号码如下。

```
0x101......640×480×8bit		彩色
0x103......800×600×8bit		彩色
0x105......1024×768×8bit	彩色
0x107......1280×1024×8bit	彩色
```

另外，在QEMU中不能指定最下面的0x107。实际指定的时候，要像在`asmhead.nas`中所做的那样，将以上的画面模式号码值**<u>加上0x4000</u>**，再赋值到BX中去。不这样做就不能顺利运行。

##### 第十五天 多任务

在一般的操作系统中，任务切换的动作每0.01～0.03秒就会进行一次。当然，切换的速度越快，让人觉得程序是在同时运行的效果也就越好。不过，CPU进行程序切换（我们称为“任务切换”）这个动作本身就需要消耗一定的时间，这个时间大约为0.0001秒左右，不同的CPU及操作系统所需的时间也有所不同。如果CPU每0.0002秒切换一次任务的话，该CPU处理能力的50%都要被任务切换本身所消耗掉。

当你向CPU发出任务切换的指令时，CPU会先把寄存器中的值全部写入内存中，这样做是为了当以后切换回这个程序的时候，可以从中断的地方继续运行。接下来，为了运行下一个程序，CPU会把所有寄存器中的值从内存中读取出来。我们前面所说的任务切换所需要的时间，正是对内存进行写入和读取操作所消耗的时间。

###### 任务状态段  task status segment

“任务状态段”（task status segment），简称TSS。TSS有16位和32位两个版本，这里我们使用32位版。顾名思义，TSS也是内存段的一种，需要在GDT中进行定义后使用。

```c
struct TSS32 {    
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;    
    int ldtr, iomap;
};
```

如果在TSS中将EIP寄存器的值记录下来，那么当下次再返回这个任务的时候，CPU就可以明白应该从哪里读取程序来运行了。

第4行的ldtr和iomap也和第1行的成员一样，是有关任务设置的部分，因此在任务切换时不会被CPU写入。也许你会想，那就和第1行一样，暂时先忽略好了——但那可是绝对不行的！如果胡乱赋值的话，任务就无法正常切换了，在这里我们先将ldtr置为0，将iomap置为0x40000000就好了。

###### JMP 指令的分类

JMP指令分为两种，只改写EIP的称为near模式，同时改写EIP和CS的称为far模式，在此之前我们使用的JMP指令基本上都是near模式的。

如果一条JMP指令所指定的目标地址段不是可执行的代码，而是TSS的话，CPU就不会执行通常的改写EIP和CS的操作，而是将这条指令理解为任务切换。也就是说，CPU会切换到目标TSS所指定的任务，说白了，就是JMP到一个任务那里去了。

CPU每次执行带有段地址的指令时，都会去确认一下GDT中的设置，以便判断接下来要执行的JMP指令到底是普通的far-JMP，还是任务切换。也就是说，从汇编程序翻译出来的机器语言来看，普通的far-JMP和任务切换的far-JMP，指令本身是没有任何区别的。

###### TR 寄存器

我们向TR寄存器存入3  *  8这个值，这是因为我们刚才把当前运行的任务定义为GDT的3号。TR寄存器以前没有提到过，<u>它的作用是让CPU记住当前正在运行哪一个任务。</u>**（而不是说下一个执行的是哪一个任务）**当进行任务切换的时候，TR寄存器的值也会自动变化，它的名字也就是“task  register”（任务寄存器）的缩写。我们每次给TR寄存器赋值的时候，必须把GDT的编号乘以8，因为英特尔公司就是这样规定的。

给TR寄存器赋值需要使用LTR指令。

```assembly
_load_tr:       ; void load_tr(int tr)
        LTR [ESP+4]
        RET
```

要进行任务切换，我们必须执行far模式的跳转指令，可惜far跳转这事C语言还是无能为力，

```assembly
_taskswitch4:	; void taskswitch4(void);
		JMP		4*8:0
		RET
```

也许有人会问，在JMP指令后面写个RET有意义吗？也对，通常情况下确实没意义，因为已经跳转到别的地方了嘛，后面再写什么指令也不会被执行了。不过，用作任务切换的JMP指令却不太一样，在切换任务之后，再返回这个任务的时候，程序会从这条JMP指令之后恢复运行，也就是执行JMP后面的RET，从汇编语言函数返回，继续运行C语言主程序。

另外，如果far-JMP指令是用作任务切换的话，地址段（冒号前面的4*8的部分）要指向TSS这一点比较重要，而偏移量（冒号后面的0的部分）并没有什么实际作用，会被忽略掉，一般来说像这样写0就可以了。

###### JMP far

```assembly
_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]				; eip, cs
		RET
```

“JMP FAR”指令的功能是执行far跳转。在JMP FAR指令中，可以指定一个内存地址，CPU会从指定的内存地址中读取4个字节的数据，并将其存入EIP寄存器，再继续读取2个字节的数据，并将其存入CS寄存器<u>**(注意是先IP后CS)**</u>。当我们调用这个函数，比如farjmp(eip,cs);，在[ESP+4]这个位置就存放了eip的值，而[ESP+8]则存放了cs的值。

###### 往 esp 中写入参数来达到向函数传递参数的目的

```c
tss_b.eip = task_b_main_addr;
tss_b.esp = memory_alloc(global_memory_manager, 64 * 1024) + 64 * 1024 - 8; // 给任务 b 准备 64kb 的栈空间
*((int *) (tss_b.esp + 4)) = 1234;
```

```c
void task_b_main(int arg) {
    char buffer[20] = {0};
    sprintf(buffer, "%d hello", arg);
}
```

![image-20210502165425140](readme.assets/image-20210502165425140.png)

###### tss 的一个坑：如果要维护某个状态的话，一定要在修改完状态之后再 switch_task

例如：

```c
int count = 0;
void callback(){
    switch_task();
    ++count;	// wrong way
}
void callback(){
    ++count;	// correct way
    switch_task();
}
```

一定要在 `switch_task` 之前对 `count` 的状态进行修改，不然的话 `count` 的状态可能永远都没机会修改了！！！（血与泪的教训）

##### 第十八天  dir 命令

###### FILEINFO 结构体

见前面的内存分布图：

```
0x00100000 - 0x00267fff : 用于保存软盘的内容
```

但是文件保存的软盘从0柱面、0磁头、1扇区开始的0x002600之后，也就是内存地址的0x00102600开始写入。

文件信息结构体

```c
struct FILEINFO {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
```

```
大小:8 + 3 + 1 + 10 + 2 * 3 + 4 = 32
```

```
name:文件名，八个字节，不足8个用空格补足
	如果第一个字节为0xe5，代表这个文件已经被删除了
	如果第一个字节为0x00，代表这一段不包含任何文件名信息
ext:拓展名，三个字节，不足3个用空格补足
type:文件的属性信息
	0x01......只读文件（不可写入）
	0x02......隐藏文件
	0x04......系统文件
	0x08......非文件信息（比如磁盘名称等）
	0x10......目录
	0x20......普通文件
reserve:保留字，以后可能会用到
time:文件的时间
date:文件的日期
clustno:代表这个文件的内容从磁盘上的哪个扇区开始存放。变量名clustno本来是“簇号”（cluster    number）的缩写，“簇”这个词是微软的专有名词，在这里我们先暂且理解为和“扇区”是一码事就好了。
size:文件的大小
```

从磁盘映像的0x004200就开始存放文件haribote.sys了，因此文件信息最多可以存放224个。

```
0x4200 - 0x2600 = 0x1c00 = 7168
7168 / 32 = 224
```

##### 第十九天 应用程序

###### 根据 `clustno` 获得文件的真实存储地址

```c
struct FILEINFO {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
```

单单看 `clustno` 这个字符

| 文件名       | raw clustno | clustno | 存储地址 |
| ------------ | ----------- | ------- | -------- |
| HARIBOTE.SYS | 02 00       | 0x0002  | 0x004200 |
| IPL10.NAS    | 39 00       | 0x0039  | 0x00b000 |
| MAKE.BAT     | 3F 00       | 0x003f  | 0x00bc00 |

 culstno每增加1，磁盘映像中的位置就增加0x200个字节，即 512 字节，恰好是一个扇区的大小。

```
磁盘映像中的地址 = clustno * 512 + 0x003e
```

###### FAT (file allocation table 文件分配表)

按照Windows管理磁盘的方法，保存大于512字节的文件时，有时候并不是存入连续的扇区中，

对于文件的下一段存放在哪里，在磁盘中是有记录的，这个记录位于从0柱面、0磁头、2扇区开始的9个扇区中，在磁盘映像中相当于0x000200～0x0013ff。这个记录被称为FAT，是“file allocation table”的缩写，翻译过来叫作“文件分配表”（即记录文件在磁盘中存放位置的表）。

不过这个FAT使用了微软公司设计的算法进行压缩，因此我们无法直接看懂里面的内容，需要先用下面的方法进行解压缩。

首先将数据以3个字节分为一组，并进行换位

```
ab cd ef → dab efc
F0 FF FF → FF0 FFF
```

![image-20210506094422764](readme.assets/image-20210506094422764.png)

```
byte0 = 0xab
byte1 = 0xcd
byte2 = 0xef
0xdab = 0xd00 | 0x0ab
	  = (0xcd00 & 0x0f00) | byte0
	  = ((0xcd << 8) & 0xf00) | byte0
	  = ((byte1 << 8) & 0xf00)| byte0
0xefc = 0xef0 | 0x00c
	  = (oxef << 4) | (oxcd >> 4)
	  = (byte2 << 4) | （byte1 >> 4)
```



|      | +0   | +1   | +2   | +3   | +4   | +5   | +6   | +7   | +8   | +9   |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 0    | FF0  | FFF  | 003  | 004  | 005  | 006  | 007  | 008  | 009  | 00A  |
| 10   | 00B  | 00C  | 00D  | 00E  | 00F  | 010  | 011  | 012  | 013  | 014  |
| 20   | 015  | 016  | 017  | 018  | 019  | 01A  | 01B  | 01C  | 01D  | 01E  |
| 30   | 01F  | 020  | 021  | 022  | 023  | 024  | 025  | 026  | 027  | 028  |
| 40   | 029  | 02A  | 02B  | 02C  | 02D  | 02E  | 02F  | 030  | 031  | 032  |
| 50   | 033  | 034  | 035  | 036  | 037  | 038  | 039  | FFF  | 03B  | 03C  |
| 60   | 03D  | 03E  | 03F  | FFF  | FFF  | 000  | 000  | 000  | 000  | 000  |

以haribote.sys为例来分析一下。已知clustno   =   2，因此我们读取0x004200～0x0043ff这512个字节。那么接下来应该读取哪里的数据呢.

我们来看FAT的第2号记录，其值为003，也就是说下面的部分存放在clustno  =  3这个位置，即读取0x004400～0x0045ff这512个字节。再接下来参照FAT的第3号记录，即可得到下一个地址clustno = 4。

那么接下来应该读取哪里的数据呢？我们来看FAT的第2号记录，其值为003，也就是说下面的部分存放在clustno  =  3这个位置，即读取0x004400～0x0045ff这512个字节。再接下来参照FAT的第3号记录，即可得到下一个地址clustno = 4。

以此类推，我们一直读取到clustno = 57（0x39），57号扇区后面应该读哪里了呢？参照对应的FAT记录，居然是FFF。也就是说，57号之后已经没有数据了，即这里就是文件的末尾。一般来说，如果遇到FF8～FFF的值，就代表文件数据到此结束。

只要其中一个地方损坏，之后的部分就会全部混乱。因此，微软公司将FAT看作是最重要的磁盘信息，为此在磁盘中存放了2份FAT。第1份FAT位于0x000200～0x0013ff，第2份位于0x001400～0x0025ff。其中第2份是备份FAT，内容和第1份完全相同。

##### 第二十天 api

###### 应用程序调用系统的函数

操作系统中有这样一个函数：

```c
// 在控制台中打印一个字符，之后回车，之后刷新
void print_char_in_console_and_redraw(struct TextBox *textBox, char ch);
```

现在应用程序想要调用这个函数，往控制台中输出一个字符，该怎么做？

应用程序需要知道三个条件：

1. `print_char_in_console_and_redraw`这个函数的地址。
2. `textBox`的值，即控制台的地址。
3. `ch`的值，即应用程序想要输出的字符。

应用程序怎样得到这三个条件？

1. `print_char_in_console_and_redraw`这个函数的地址。

   这个函数的地址在 `bootpack.map` 中可以找到：

   ```
   0x00001CB8 : _print_char_in_console_and_redraw
   ```

2. `textBox`的值。

   扑街，因为 textBox 这个变量是在操作系统内部生成的，所以外部引用没办法得到他的地址。

3. `ch`的值

   应用程序想要打印哪个字符，还不是由它自己说了算？

因为没办法知道，textBox 的值，所以应用程序没办法直接调用 `print_char_in_console_and_redraw`这个函数。

该怎么解决一个问题呢？

1. 我的解法

   其实可以对这个函数`print_char_in_console_and_redraw`包装一下。将包装后的函数明明为`print_char_in_console_and_redraw_wrapper`。这个 wrapper 只需要一个参数就能执行，它的实现类似这样：

   ```c
   // 在控制台中打印一个字符，之后回车，之后刷新
   void print_char_in_console_and_redraw_wrapper(char ch){
       struct TextBox *textBox = getTextBoxAddress();
       print_char_in_console_and_redraw(textBox,ch);
   }
   ```

   这样外部的应用程序只需要知道 `print_char_in_console_and_redraw_wrapper`和地址，并自己提供一个 char，就可以启动这个函数了。

   > 可是怎么传参又成了一个问题，刚刚试了一下。
   >
   > ```asm
   > PUSH 'A'
   > CALL 2*8:0x1CDC  ; print_char_in_console_and_redraw_wrapper 的地址
   > ADD	ESP,4
   > ```
   >
   > 发现有问题，算了还是用书上的方法吧。

2. 书上的解法

   用汇编实现了一个函数

   ```assembly
   ; naskfunc.nas
   ; void print_char_in_console_and_redraw(struct TextBox *textBox, char ch);
   _asm_print_char_in_console_and_redraw:
   		AND		EAX,0xff ; 因为我们只需要 AX 就够了（最后的十六个 bit）
   		PUSH	EAX      ; 推入第二个参数 ch
   		PUSH	DWORD [0x0fec]	; 推入第一个参数 textBox 的地址
   		CALL	_print_char_in_console_and_redraw
   		ADD		ESP,8	 ; 将栈寄存器还原
   		RETF    ; far return 注意
   ```

   调用者把想要打印的字符赋值给`AX`寄存器。之后调用该函数即可。其中`0x0fec`是存储 <u>控制台窗口地址</u> 的地址。

   ```c
   // bootpack.c
   struct TextBox *textBox = newTextBox(console_window, border, title_bar_height, window_weight - border,window_height - border);
   *(struct TextBox **) (0x0fec) = textBox;
   ```

   应用程序的代码：

   ```assembly
   ; hlt.nas
   MOV	AL,'A'
   CALL 2*8:0xA01  ; _asm_print_char_in_console_and_redraw 的地址
   ```

   **注意：**

   1. 因为操作系统在2号段上，而应用程序在 1003 号段上，所以不能简单用 call，而应该用跨段的 call，即 call 的时候加上段号。
   2. 因为调用时使用的是 call far，所以在 `_asm_print_char_in_console_and_redraw` 用的 `retf` 返回。

###### 操作系统执行应用程序并返回

之前我们执行应用程序的方式设这样的：

```c
jmp_to_code_segment(0);
```

直接 `jmp` 到应用程序的代码段地址上去了，并且一去不复返~

如果想执行完程序之后返回的话，就必须使用`call`和`ret`这个combo了。

此外，由于应用程序和操作系统不在一个段上，因此需要用到`call far`和`retf`。

`call far` 的实现与 `jmp far` 基本一致

```assembly
; naskfunc.nas
_jmp_far:       ; void jmp_far(int eip,int cs);
        JMP far [ESP+4]
        RET

_call_far:      ; void call_far(int eip,int cs);
        call far [ESP+4]
        RET
```

##### 第二十一天 保护操作系统

###### 用C语言编写应用程序 

用 c 语言编写应用程序和之前用汇编写应用程序其实差不多，反正最终生成的都是二进制啦。

```c
// a.c
void print_char(char c);

void HariMain(void) {
    print_char('b');
    return;
}
```

```assembly
; a_nask.nas
[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令
[BITS 32]						; 生成32位模式机器语
[FILE "a_nask.nas"]				; 源文件名信息

GLOBAL _print_char

[SECTION .text]

_print_char:	; void print_char(char c);
    MOV	AL,[ESP+4]
    INT	0x40
    RET
```

把他们链接，编译成二进制然后打包进镜像即可。有可能坑的是必须修改二进制的前六个字节，程序才能正常执行。

```c
buffer[0] = 0xe8;
buffer[1] = 0x16;
buffer[2] = 0x00;
buffer[3] = 0x00;
buffer[4] = 0x00;
buffer[5] = 0xcb;
```

###### 对数据段和栈段的理解

**数据段 data segment**

```assembly
mov ax,[1234]
```

将地址为 1234 的数据赋值给 ax 寄存器。

但是这个 1234 是一个绝对的内存地址么？

No，它只是当前程序所在数据段上的一个决定地址（可以把它理解成一个相对地址）

实际上这个 1234 访问的是程序当前所在的数据段上的 1234 地址。

ds 是一个寄存器，data segment，他记录了当前的数据段的值。

所以实际上这段代码等于 

```assembly
mov ax,ds:1234
```

同理还有这么一段 c 代码

```c
char c = *(char*)0x1234
```

他实际上访问的也是程序当前所在的数据段的 1234 号地址。

看看下面这段汇编：

```assembly
mov ax,1
mov ds,ax
mov bx,[1234]

mov ax,2
mov ds,ax
mov cx,[1234]
```

虽然都访问地址都是 1234，但是最终 bx 和 cx 的值是不同的，因为 ds 改变了，所以实际上访问的是不同的地址了。

**栈段 stack segment**

```assembly
MOV sp,4567
PUSH ax
```

首先设置了 sp 的值，值将 ax 寄存器的值推入栈内，那么到底存入了哪里。

因为将 sp 设置成了 4567，因为 ax 的值被存入到 4568 和 4569 这两个字节里。

但是这两个字节是绝对地址么？

No，依旧是一个相对地址。

与前面不同之处在于，这次是相对于栈段的地址。

程序怎么知道自己的栈段？

看 ss 寄存器，它记录了程序当前的栈段。

所以实际上 `PUSH ax`

约等于

```assembly
MOV ss:sp,ax
ADD sp,2
```

看这么一段代码

```assembly
MOV ax,1
MOV ss,ax

PUSH bx
POP bx
```

执行这段代码后，在 POP 之后，bx 变成了原来的值。

```assembly
mov ax,1
MOV ss,ax

PUSH bx

MOV ax,2
mov ss,ax

POP bx
```

POP 之后，bx 的值并没有变成原来的值，虽然 sp 没变，但是因为 ss 已经变了，POP 时访问的实际内存和 PUSH 时访问的实际内存已经不同了。

###### 保护操作系统

```c
// crack1.c
void HariMain(void){
    *((char *) 0x00102600) = 0;    
    return;
}
```

目前操作系统没有对内存进行保护，是无法阻止应用程序写内存写东西的。

**如何阻止？**

它所干的坏事，其实就是擅自访问了本该由操作系统来管理的内存空间。我们需要为应用程序提供专用的内存空间。

要做到这一点，我们可以创建应用程序专用的数据段，<u>并在应用程序运行期间，将DS和SS指向该段地址</u>。

DS 即 data segment，数据段。

SS 即 stack segment，栈段。

```
操作系统用代码段......2 * 8
操作系统用数据段......1 * 8
应用程序用代码段......1003 * 8
应用程序用数据段......1004 * 8
(3 * 8～1002 * 8为TSS所使用的段)
```

首先梳理一下我们之前是怎样执行 hlt 这个应用程序的 ，这是 hlt 的代码

```assembly
[BITS 32]
MOV	AL,'A'
INT 0x40  ; 通过触发 0x40 号中断来间接调用 _asm_print_char_in_console_and_redraw
RETF
```

虽然此时这个程序的作用并不是 hlt 了，但是我还是叫他 hlt 吧。

这是 bootpack.c 中执行 hlt 的代码

```c
struct FileInfo *file = FindFileByName("HLT.HRB");
char *buffer = (char *) memory_alloc(global_memory_manager, 4096);
ReadFileIntoBuffer(file, buffer, 4096);
set_segment_desc(1003, 4096 - 1, buffer, AR_CODE32_ER);
call_far(0, 1003 * 8);
```

```
操作系统把 HLT.HRB 读取中 buffer 内
操作系统把 buffer 设置为 1003 段
操作系统执行 1003 号段的程序
应用程序执行结束后返回到操作系统
```

对于应用程序

```
应用程序给 AL 寄存器赋值
应用程序触发 40 号中断，间接调用操作系统的 _asm_print_char_in_console_and_redraw 函数
操作系统的 _asm_print_char_in_console_and_redraw 函数 调用 操作系统的 print_char_in_console_and_redraw 函数
_asm_print_char_in_console_and_redraw 返回后，重新回到应用程序
```

回城时序图大概是这样的：

```mermaid
sequenceDiagram
	participant 操作系统
	participant 应用程序
	操作系统 ->> 操作系统 : set_segment_desc
	操作系统 ->> 应用程序 : call_far
	应用程序 ->> 操作系统 : asm_print_char_in_console_and_redraw
    操作系统 ->> 操作系统 : print_char_in_console_and_redraw
    操作系统 ->> 应用程序 : return
    应用程序 ->> 操作系统 : return
```

再来看看这段程序
```c
// crack1.c
void HariMain(void){
    *((char *) 0x00102600) = 0;    
    return;
}
```

```c
set_segment_desc(1003, 4096 - 1, buffer, AR_CODE32_ER);
call_far(0, 1003 * 8);
```

通过 call_far 来执行应用程序时，实际上是将 cs 设置成了 1003，ip 设置成了 0。

但是 ds 和 ss 却并没有变，因此应用程序在执行过程中，使用的依旧是操作系统的 ss 和 ds。

这就导致，执行这句代码时

```c
*((char *) 0x00102600) = 0;
```

由于 ds 是操作系统的 ds，因此它修改了操作系统的数据段中的内容，从而破坏了操作系统的结构！

所以只要为应用程序准备一个单独的数据段和栈段，并修改 ds 和 ss，之后再去执行应用程序就 ok 了。

道理就类似于班上有个小朋友很调皮，经常在班上捣蛋，影响了其他小朋友学习，然后老师就把他调到了一间空教室，这样不管他再调皮，也没办法影响别人了。

这间空教室就是我们为应用程序准备的 ds 和 ss。

还有一点要注意的是：在应用程序执行完毕后，需要将 ds 和 ss 恢复到操作系统原来的 ds 和 ss，毕竟操作系统还是要正常运转的，如果操作系统运行过程中访问的是应用程序的 ds 和 ss，那世界就有乱套了。

```
应用程序用代码段......1003 * 8
应用程序用数据段......1004 * 8
```

我们将 1003 号段作为应用程序专用的代码段，将 1004 号段作为应用程序专用的数据段/栈段。

```c
char *codeSegment = (char *) memory_alloc(global_memory_manager, 4096);
set_segment_desc(gdt + 1003, limit, addr, AR_CODE32_ER);
char *dataSegment = (char *) memory_alloc(global_memory_manager, 64 * 1024);
set_segment_desc(gdt + 1004, limit, addr, AR_DATA32_RW);
```

之后就是执行应用程序了，之前我们是直接使用 call_far 跳到 1003 号段去执行应用程序了。

```c
call_far(0, 1003);
```

```assembly
_call_far:      ; void call_far(int eip,int cs);
        call far [ESP+4]
        RET
```

但是现在不行了，因为我们在执行应用程序之前，需要将 ds 和 ss 设置为我们所准备的段上。

因此创建了一个 start_app 函数。

```c
void start_app(int eip, int cs, int esp, int ds);
```

参数 ds 用来指定 ds 寄存器和 cs 寄存器的值。

参数 esp 用来指定 esp 寄存器的值，esp 寄存器是用来指定栈顶位置的。

参数 cs 用来指定 cs 寄存器的值。

参数 eip 用来指定 eip 寄存器的值。

cs 寄存器和 eip 寄存器决定系统下一条指令的地址。

start_app 的作用是，将 ds 和 ss 和 esp 设置为我们所指定的值后，之后执行 eip 和 cs 处的应用程序，待应用程序执行完毕后，再将 ds，ss 和 esp 恢复为原来的值，之后返回。

```c
char *codeSegment = (char *) memory_alloc(global_memory_manager, 4096);
set_segment_desc(gdt + 1003, 4096 - 1, addr, AR_CODE32_ER);
char *dataSegment = (char *) memory_alloc(global_memory_manager, 64 * 1024);
set_segment_desc(gdt + 1004, 64 * 1024 - 1, addr, AR_DATA32_RW);
start_app(0, 1003 * 8, 64 * 1024, 1004 * 8);
```

因为 start_app 需要操作寄存器，所以肯定只能用汇编来实现。

```assembly
_start_app:     ; void start_app(int eip, int cs, int esp, int ds);
    PUSHAD      ; 4 * 8，8个寄存器，每个寄存器 4 个字节

    MOV EAX,[ESP+36]    ; EAX = eip
    MOV ECX,[ESP+40]    ; ECX = cs
    MOV EDX,[ESP+44]    ; EDX = esp
    MOV EBX,[ESP+48]    ; EBX = ds

    CLI                 ; 禁止中断

    MOV [0xfe4],ESP     ; 操作系统用ESP

    MOV DS,BX           ; DS = ds
    MOV SS,BX           ; SS = ds
    MOV ESP,EDX         ; ESP = esp

    PUSH ECX            ; PUSH cs       for far call（设置好 SS 和 SP 之后，这个地方其实已经是往函数的栈PUSH了，而不是系统的栈）
    PUSH EAX            ; PUSH eip      for far call（设置好 SS 和 SP 之后，这个地方其实已经是往函数的栈PUSH了，而不是系统的栈）
    CALL FAR [ESP]      ; 调用应用程序

    MOV AX,1*8         ; 操作系统的 SS，DS
    MOV DS,AX
    MOV SS,AX

    MOV ESP,[0xfe4]     ; 还原 ESP

    STI                 ; 恢复中断

    POPAD       ; 还原寄存器的值
    RET
```

ok，现在应用程序在我们为他准备的空教室去活动了。但是应用程序在执行过程中，会触发 0x40 号中断，进入到 `_asm_print_char_in_console_and_redraw` 中，但此次ss和ds依旧是应用程序的 ss 和 ss，之后`_asm_print_char_in_console_and_redraw` 会调用操作系统`_print_char_in_console_and_redraw`来进行输出，因此在调用 `_print_char_in_console_and_redraw`前，需要将 ss 和 ds 设置为操作系统的 ss 和 ds。

```assembly
_asm_print_char_in_console_and_redraw:
		PUSHAD           ; 记录所有 32 位寄存器的值

		AND		EAX,0xff ; ax = ch，因为我们只需要 AX 就够了（最后的十六个 bit）

		MOV     BX,DS    ; BX = SS，记录应用程序所在的数据段
		MOV     CX,SS    ; CX = SS, 记录应用程序所在的栈段

		MOV     DX,1*8   ; 操作系统所在的段 调整 ES,DS
		MOV     SS,DX    ; SS = 1 * 8
		MOV     DS,DX    ; DS = 1 * 8

		MOV     EDX,ESP   ; EDX = ESP，记录应用程序的栈顶

		MOV     ESP,[0xfe4] ; 将 ESP 调整到操作系统的 ESP, 因为前面已经将 DS 调整到操作系统所在的段了，这个地方的 0xfe4 实际上是操作系统数据段上的 0xfe4

        PUSH    BX       ; 这三个 PUSH 主要是防止 这三个寄存器在 _print_char_in_console_and_redraw 函数内被修改了
        PUSH    CX       ; 这三个 PUSH 主要是防止 这三个寄存器在 _print_char_in_console_and_redraw 函数内被修改了
        PUSH    EDX      ; 这三个 PUSH 主要是防止 这三个寄存器在 _print_char_in_console_and_redraw 函数内被修改了

        PUSH	EAX                     ; 推入第二个参数 ch
        PUSH	DWORD [0x0fec]	        ; 推入第一个参数 textBox 的地址
		CALL	_print_char_in_console_and_redraw
		ADD     ESP,8

		POP     EDX       ; 还原 BX,CX,EDX
		POP     CX        ; 还原 BX,CX,EDX
		POP     BX        ; 还原 BX,CX,EDX

		MOV     DS,BX     ; 还原应用程序的数据段
		MOV     SS,CX     ; 还原应用程序的栈段
		MOV     ESP,EDX   ; 还原 ESP

		POPAD           ; 还原所有 32 位寄存器的值
		IRETD           ; 使用INT指令来调用的时候会被视作中断来处理，用 RETF 是无法返回的，需要使用 IRETD 指令
```

还有一点要注意的是，在执行应用程序的过程后，也是有可能发生硬件中断的。

```assembly
; 定时器相关
_asm_int_handler20:
    PUSH    ES
    PUSH    DS
    PUSHAD          ; 上面三个 push 会把寄存器的值都存起来，包括 EAX，ESP

    MOV     EAX,ESP ;  这两句代码是用来干啥的，没看懂
    PUSH    EAX     ;  这两句代码是用来干啥的，没看懂

    MOV     AX,SS   ;   将 DS 和 ES 调整到与 SS 相等
    MOV     DS,AX   ;   将 DS 和 ES 调整到与 SS 相等
    MOV     ES,AX   ;   将 DS 和 ES 调整到与 SS 相等
    CALL    _int_handler20

    POP     EAX

    POPAD           ; 下面三个 pop 会把寄存器的值都弹出去
    POP     DS
    POP     ES
    IRETD
```

这是我们之前处理定时器中断的代码，当定时器中断发生时，会进入到 `_asm_int_handler20`内，`_asm_int_handler20`会调用操作系统`_int_handler20`，这里就有可能发生一个问题。

如果中断是在执行应用程序的过程中发生的，那么在 `CALL    _int_handler20`时，ss 和 ds 寄存器的值可能是应用程序的栈段和数据段，这样的话，调用 int_handler20 就会发生问题。

所以需要在`CALL    _int_handler20`将ss 和 ds 寄存器设置为操作系统的值。（但我懒得做了）

###### 对异常的支持

要想强制结束程序，只要在中断号0x0d中注册一个函数即可，这是因为在x86架构规范中，当应用程序试图破坏操作系统，或者试图违背操作系统的设置时，就会自动产生0x0d中断，因此该中断也被称为“异常”。

因为只要为 0x0d 中断设置一个 handler 就好了。我们可以在 handler 中，往控制台输出一些异常信心，并将 cs，ip，ds，cs，sp寄存器的值 还原到操作系统原来的值。这样就能终止异常程序了。

###### 道高一尺 魔高一丈

如果坏蛋在应用程序中强行修改 ds 到操作系统所在的段，那我们就又 gg 了。

```assembly
[INSTRSET "i486p"]
[BITS 32]        
MOV     AX,1*8         ; OS用的段号        
MOV     DS,AX          ; 将其存入DS        
MOV     BYTE [0x102600],0        
RETF
```

所以必须阻止应用程序无法使用操作系统的段地址。

x86架构正好有这样的功能。

<u>在段定义的地方，如果将访问权限加上0x60的话，就可以将段设置为应用程序用。</u>

当CS中的段地址为应用程序用段地址时，CPU会认为“当前正在运行应用程序”，这时如果存入操作系统用的段地址就会产生异常。

这又出现了一个问题，cpu 怎么知道某个段是操作系统的段？







