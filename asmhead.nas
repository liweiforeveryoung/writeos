; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; 安装bootpack的目的地
DSKCAC	EQU		0x00100000		; 磁盘缓存位置
DSKCAC0	EQU		0x00008000		; 磁盘缓存位置（实模式）

; Boot_info
CYLS        EQU     0x0ff0      ; 磁盘的柱面数， ipl10.nas 中将柱面数写入了该地址
LEDS        EQU     0x0ff1      ; 指示灯的状态
VMODE       EQU     0x0ff2      ; 关于颜色数目的信息，颜色的位数
SCRNX       EQU     0x0ff4      ; 分辨率的 x (screen x)
SCRNY       EQU     0x0ff6      ; 分辨率的 y (screen y)
VRAM        EQU     0x0ff8      ; 图像缓冲区的开始地址
ORG		0xc200      ; 起始地址
MOV     AL,0x13     ; VGA 显卡，320 * 300 * 8 位彩色
MOV     AH,0x00
INT     0x10
MOV     BYTE [VMODE],8  ; 记录画面模式
MOV     WORD [SCRNX],320
MOV     WORD [SCRNY],200
MOV     DWORD [VRAM],0x000a0000 ; 在 VGA 模式下，VRAM是0xa0000～0xaffff的64KB
; 用BIOS取得键盘上各种LED指示灯的状态
MOV     AH,0x02
INT     0x16
MOV     [LEDS],AL
; PIC 关闭一切中断
;   根据AT兼容机的规格，如果要初始化PIC
;   必须在CLI之前进行，否则有时会挂起
;   随后进行PIC的初始化

		MOV		AL,0xff
		OUT		0x21,AL         ; 禁止主PIC的全部中断 等同于 io_out(PIC0_IMR, 0xff)
		NOP						; 如果连续执行OUT指令，有些集中会无法正常运行 NOP命令什么都不做，只是让 CPU 休息一个时钟长的时间
		OUT		0xa1,AL         ; 禁止从PIC的全部中断 等同于 io_out(PIC1_IMR, 0xff)

		CLI						; 禁止CPU级别的中断

; 为了让CPU能够访问1MB以上的内存空间，设定A20GATE

		CALL	waitkbdout      ;  wait_KBC_sendready()
		MOV		AL,0xd1
		OUT		0x64,AL         ;  io_out8(KEY_CMD_PORT, KEY_CMD_WRITE_OUT_PORT)
		CALL	waitkbdout      ;  wait_KBC_sendready()
		MOV		AL,0xdf			;  enable A20
		OUT		0x60,AL         ;  io_out8(KEY_DAT_PORT, KBC_OUT_PORT_A20G_ENABLE
		CALL	waitkbdout      ;  wait_KBC_sendready()

; 切换到保护模式

[INSTRSET "i486p"]				; 想要使用 486 指令的描述 为了能够使用386以后的LGDT，EAX，CR0等关键字

		LGDT	[GDTR0]			; 临时设定 GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设 bit32 为 0（为了禁止分页）
		OR		EAX,0x00000001	; 设 bit0 为 1（为了切换到保护模式）
		MOV		CR0,EAX         ; 将 CR0 的最高位设置为 0，最低位设置为 1；CR0 也就是control register 0，是一个非常重要的寄存器，只有操作系统才能操作它。
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			; 可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的转送

		MOV		ESI,bootpack	; 转送源
		MOV		EDI,BOTPAK		; 转送目的地
		MOV		ECX,512*1024/4
		CALL	memcpy

; 磁盘数据最终转送到它本来的位置去

; 首先从启动扇区开始

		MOV		ESI,0x7c00		; 转送源
		MOV		EDI,DSKCAC		; 转送目的地
		MOV		ECX,512/4
		CALL	memcpy

; 所有剩下的

		MOV		ESI,DSKCAC0+512	; 转送源       在 ipl10 中把磁盘内容读到了 0x8200
		MOV		EDI,DSKCAC+512	; 转送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 从柱面数变换为字节数 / 4
		SUB		ECX,512/4		; 减去 IPL
		CALL	memcpy

; 必须由 asmhead 完成的工作，至此全部完成
;	以后就交给 bootpack 来完成

; bootpack 的 启动，这个地方没看懂哇！！

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]    ;                   [EBX + 16]......bootpack.hrb之后的第16号地址。值是0x11a8
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 没有要转送的东西时
		MOV		ESI,[EBX+20]	; 转送源            [EBX + 20]......bootpack.hrb之后的第20号地址。值是0x10c8
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 转送目的地         [EBX + 12]......bootpack.hrb之后的第12号地址。值是0x00310000
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈初始值
		JMP		DWORD 2*8:0x0000001b ; 这里的0x1b号地址是指第2个段的0x1b号地址。第2个段的基地址是0x280000，所以实际上是从0x28001b开始执行的。这也就是 bootpack.hrb的0x1b号地址。这样就开始执行bootpack.hrb了。■■■■■下面要讲的内容可能有点偏离主题，但笔者还是想介绍一下“纸娃娃系统”的内存分布图。0x00000000  -  0x000fffff  : 虽然在启动中会多次使用，但之后就变空。（1MB）0x00100000 - 0x00267fff : 用于保存软盘的内容。（1440KB）0x00268000 - 0x0026f7ff : 空（30KB）0x0026f800 - 0x0026ffff : IDT（2KB）0x00270000 - 0x0027ffff : GDT（64KB）


waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN       AL,0x60        ; 空读（为了清空数据接收缓冲区中的垃圾数据（如果控制器里有键盘代码，或者是已经累积了鼠标数据，就顺便把它们读取出来。）
		JNZ		waitkbdout		; AND的结果如果不是0，就跳到 waitkbdout
		RET
; memcpy(转送源地址, 转送目的地址, 转送数据的大小) 转送数据大小是以双字为单位的，所以数据大小用字节数除以4来指定。
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy
		RET

		ALIGNB	16  ; ALIGNB指令的意思是，一直添加 DB 0，直到时机合适的时候为止。
                    ; 什么是“时机合适”呢？大家可能有点不明白。ALIGNB 16 的情况下，地址能被16整除的时候，就称为“时机合适”。
                    ; 如果最初的地址能被16整除，则 ALIGNB 指令不作任何处理
                    ; 如果标签GDT0的地址不是8的整数倍，向段寄存器复制的MOV指令就会慢一些。
GDT0:
		RESB	8				; null seclector GDT0也是一种特定的GDT。0号是空区域（null   sector），不能够在那里定义段。
		DW		0xffff,0x0000,0x9200,0x00cf	; Read / write segment 32bit      set_segmdesc(gdt + 1,0xffffffff,0x00000000,AR_DATA32_RW);
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行段32bit（用于 bootpack ）  set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
