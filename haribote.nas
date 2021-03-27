; haribote-os
; TAB=4
; Boot_info
CYLC        EQU     0x0ff0      ; 设置启动区
LEDS        EQU     0x0ff1      ; 指示灯的状态
VMODE       EQU     0x0ff2      ; 关于颜色数目的信息，颜色的位数
SCRNX       EQU     0x0ff4      ; 分辨率的 x (screen x)
SCRNY       EQU     0x0ff6      ; 分辨率的 y (screen y)
VRAM        EQU     0x0ff8      ; 图像缓冲区的开始地址
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
fin:
    HLT
    JMP     fin