; haribote-os
; TAB=4
ORG     0xc200
MOV     AL,0x13     ; VGA 显卡，320 * 300 * 8 位彩色
MOV     AH,0x00
INT     0x10
fin:
    HLT
    JMP     fin