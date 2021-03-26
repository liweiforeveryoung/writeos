; hello-os
; TAB=4
ORG     0x7c00      ; 指明程序的装载地址，ORG 把程序装载到内存中的指定位置，为何是 0x7c00，见内存分布图
; 以下这段是标准FAT12格式软盘专用的代码
JMP     entry
DB      0x90
DB  "HELLOIPL"  ; 启动区的名字可以使任意的字符串
DW  512     ; define word 每个扇区（sector）的大小（必须是512字节）
DB  1       ; 簇（cluster）的大小（必须为1个扇区）
DW  1       ; FAT 的起始位置（一般从第一个扇区开始）
DB  2       ; FAT 的个数（必须是 2）
Dw  224     ; 根目录的大小（一般设成224 项）
DW  2880    ; 磁盘的大小 （必须是 2880 扇区）
DB  0xf0    ; 磁盘的种类 （必须是 0xf0）
DW  9       ; FAT 的长度（必须是 9 扇区）
DW  18      ; 1 个磁道 (track) 有几个扇区（必须是 18）
Dw  2       ; 磁头数（必须是 2）
DD  0       ; define double word 不使用分区（必须是 0）
DD  2880    ; 重写一次磁盘大小
DB  0,0,0x29        ; 意义不明 固定
DD  0xffffffff      ; （可能是）卷标号码
DB  "HELLO-OS   "   ; 磁盘的名称 (11 字节）
DB  "FAT12   "      ; 磁盘格式名称（8 字节）
RESB    18              ; 先空出 18 字节
;程序主体
entry:
    MOV AX,0
    MOV SS,AX
    MOV SP,0x7c00   ; 注意程序开头的 ORG 0x7c00
    MOV DS,AX
    MOV ES,AX
    MOV SI,msg
putloop:
    MOV AL,[SI]
    ADD SI,1
    CMP AL,0        ; 注意第 51 行，字符串的最后一个字符是 0

    JE fin
    MOV AH,0x0e     ; 显示一个文字
    MOV BX,15       ; 指定字符颜色
    INT 0x10        ; 调用终端
    JMP putloop
fin:
    HLT             ; halt , HLT是让CPU停止动作的指令，不过并不是彻底地停止（如果要彻底停止CPU的动作，只能切断电源），而是让CPU进入待机状态。只要外部发生变化，比如按下键盘，或是移动鼠标，CPU就会醒过来，继续执行程序。
    JMP fin
msg:
; 信息显示部分
DB  0x0a, 0x0a		; 2个换行
DB  "hello, world"
DB  0x0a			; 换行
DB  0
RESB    0x7dfe-$		; 填写 0x00,知道 0x7dfe，$ 表示这一行现在的字节数
DB  0x55, 0xaa


; INT 10 中断调用
; 显示一个字符A
; H=0x0e;
; AL=character code
; BH=0
; BL=color code
;返回值：无
