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
GLOBAL _load_cr0,_store_cr0
GLOBAL _asm_int_handler20,_asm_int_handler21,_asm_int_handler2c,_asm_int_handler27
GLOBAL _asm_memory_is_valid
GLOBAL _load_tr
GLOBAL _jmp_far,_call_far
GLOBAL _asm_print_char_in_console_and_redraw
GLOBAL _start_app
EXTERN _int_handler20,_int_handler21,_int_handler2c,_int_handler27
; _print_char_in_console_and_redraw 在 task.h 中
EXTERN _print_char_in_console_and_redraw

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
    MOV     [ESP+6],AX      ;
    LGDT    [ESP+6]
    RET


_load_idtr: ; void load_idtr(int limit,int addr)    interrupt descriptor
    MOV		AX,[ESP+4]		; limit
    MOV		[ESP+6],AX      ;
    LIDT	[ESP+6]
    RET

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

; 中断处理完成之后，不能执行“return;”（=RET指令），而是必须执行 IRETD 指令，因此必须用汇编来完成
_asm_int_handler21:
    PUSH    ES
    PUSH    DS
    PUSHAD          ; 上面三个 push 会把寄存器的值都存起来，包括 EAX，ESP

    MOV     EAX,ESP ;  这两句代码是用来干啥的，没看懂
    PUSH    EAX     ;  这两句代码是用来干啥的，没看懂

    MOV     AX,SS   ;   将 DS 和 ES 调整到与 SS 相等
    MOV     DS,AX   ;   将 DS 和 ES 调整到与 SS 相等
    MOV     ES,AX   ;   将 DS 和 ES 调整到与 SS 相等
    CALL    _int_handler21

    POP     EAX

    POPAD           ; 下面三个 pop 会把寄存器的值都弹出去
    POP     DS
    POP     ES
    IRETD

_asm_int_handler2c:
    PUSH    ES
    PUSH    DS
    PUSHAD

    MOV     EAX,ESP
    PUSH    EAX

    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    _int_handler2c

    POP     EAX

    POPAD
    POP     DS
    POP     ES
    IRETD

_asm_int_handler27:
     PUSH    ES
     PUSH    DS
     PUSHAD

     MOV     EAX,ESP
     PUSH    EAX

     MOV     AX,SS
     MOV     DS,AX
     MOV     ES,AX
     CALL    _int_handler27

     POP     EAX

     POPAD
     POP     DS
     POP     ES
     IRETD

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:       ; void load_tr(int tr)
        LTR [ESP+4]
        RET

_taskswitch3:	; void taskswitch3(void);
        JMP		3*8:0
        RET

_taskswitch4:	; void taskswitch4(void);
		JMP		4*8:0
		RET

_jmp_far:       ; void jmp_far(int eip,int cs);
        JMP far [ESP+4]
        RET

_call_far:      ; void call_far(int eip,int cs);
        call far [ESP+4]
        RET

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

; void print_char_in_console_and_redraw(struct TextBox *textBox, char ch);
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

; 启动程序
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