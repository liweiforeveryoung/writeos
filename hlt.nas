[BITS 32]
MOV	AL,'A'
INT 0x40  ; 通过触发 0x40 号中断来间接调用 _asm_print_char_in_console_and_redraw
RETF