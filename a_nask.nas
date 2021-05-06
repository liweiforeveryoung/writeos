[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令
[BITS 32]						; 生成32位模式机器语
[FILE "a_nask.nas"]				; 源文件名信息

GLOBAL _print_char

[SECTION .text]

_print_char:	; void print_char(char c);
    MOV	AL,[ESP+4]
;    MOV	AL,'B'
    INT	0x40
    RET
