; naskfunc
; TAB = 4
[FORMAT "WCOFF"]        ; 制作目标文件的格式
[INSTRSET "i486p"]      ; 作用见 readme
[BITS 32]               ; 制作32为模式用的机械语言
; 制作目标文件的信息
[FILE "naskfunc.nas"]   ; 源文件名信息

; 程序中包含的函数名
GLOBAL  _io_hlt,_write_mem8


; 以下是实际的函数
[SECTION .text]         ; 目标文件中写了这些之后再写程序
_io_hlt:        ; void io_hlt(void);
    HLT
    RET

_write_mem8:         ; void write_mem8(int addr,int data)，因为只将 data 的低 8 位写入了地址
    MOV     ECX,[ESP+4]
    MOV     AL,[ESP+8]
    MOV     [ECX],AL
    RET
