[BITS 32]
MOV	AL,'A'
CALL 2*8:0xA06  ; _asm_print_char_in_console_and_redraw 的地址
RETF