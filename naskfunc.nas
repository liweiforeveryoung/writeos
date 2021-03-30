; naskfunc
; TAB = 4
[FORMAT "WCOFF"]        ; 制作目标文件的格式
[INSTRSET "i486p"]      ; 作用见 readme
[BITS 32]               ; 制作32为模式用的机械语言
; 制作目标文件的信息
[FILE "naskfunc.nas"]   ; 源文件名信息

; 程序中包含的函数名
GLOBAL _io_hlt,_io_cli,_io_sti,_io_stihlt
GLOBAL _io_load_eflags,_io_store_eflags
GLOBAL _io_in8,_io_in16,_io_in32
GLOBAL _io_out8,_io_out16,_io_out32
GLOBAL _load_gdtr, _load_idtr


; 以下是实际的函数
[SECTION .text]         ; 目标文件中写了这些之后再写程序
_io_hlt:        ; void io_hlt(void);
    HLT
    RET

_io_cli:            ; void io_cli(void)
    CLI
    RET

_io_sti:            ; void io_sti(void)
    STI
    RET

_io_stihlt:         ; void stihlt(void)
    STI
    HLT
    RET

_io_load_eflags:        ; int io_load_eflags(void)
    PUSHFD          ; 将 flags 推入栈中
    POP     EAX     ; eax 作为返回值
    RET

_io_store_eflags:       ; void io_store_eflags(int flag)
    MOV     EAX,[ESP+4]
    PUSH    EAX
    POPFD           ; 给 flags 赋值
    RET

_io_in8:        ; int io_in8(int port) 从 port 中读入数据
    MOV     EDX,[ESP+4]
    MOV     EAX,0       ; 将 EAX 初始化，避免返回值被污染
    IN      AL,DX       ; 将端口的值读入到 AL 中，这个地方好像不知道为啥不能用 EDX，会报错，难道是端口的地址最大为 65535 ？
    RET


_io_in16:       ; int io_in16(int port)
    MOV     EDX,[ESP+4]
    MOV     EAX,0       ; 将 EAX 初始化，避免返回值被污染
    IN      AX,DX       ; 将端口的值读入到 AX 中
    RET

_io_in32:       ; int io_in32(int port)
    MOV     EDX,[ESP+4]
    MOV     EAX,0       ; 将 EAX 初始化，避免返回值被污染
    IN      EAX,DX      ;
    RET

_io_out8:       ; void io_out(int port,int data)
    MOV     EDX,[ESP+4]     ; port
    MOV     AL,[ESP+8]     ; data
    OUT     DX,AL           ; 同上端口的地址不能用 EDX，只能用 DX
    RET

_io_out16:
    MOV     EDX,[ESP+4]     ; port
    MOV     AX,[ESP+8]     ; data
    OUT     DX,AX
    RET

_io_out32:
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,[ESP+8]     ; data
    OUT     DX,EAX
    RET

_load_gdtr: ; void load_gdtr(int limit,int addr)    segment descriptor 见 readme
    MOV     AX,[ESP+4]      ; limit
    MOV     [ESP+6],AX      ; todo ???
    LGDT    [ESP+6]
    RET


_load_idtr: ; void load_idtr(int limit,int addr)    interrupt descriptor
    MOV		AX,[ESP+4]		; limit
    MOV		[ESP+6],AX      ; todo ???
    LIDT	[ESP+6]
    RET