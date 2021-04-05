#所涉及到的目标(obj)文件集合
OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj keybuffer.obj kbc.obj mousedecoder.obj global.obj

TOOLPATH	= ../z_tools/
MAKE		= $(TOOLPATH)make.exe -r
NASK		= $(TOOLPATH)nask.exe
EDIMG		= $(TOOLPATH)edimg.exe
IMGTOL		= $(TOOLPATH)imgtol.exe
COPY		= copy
DEL			= del
OSNAME		= haribote
IPLNAME		= ipl10

INCPATH		= ../z_tools/haribote/
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul


hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

# bootpack 用到了 naskfunc 中的函数，因此需要将它两  link 起来
bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

# 拼接 asmhead.bin 和 bootpack.hrb
$(OSNAME).sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb haribote.sys

$(IPLNAME).bin : $(IPLNAME).nas Makefile
	$(NASK) $(IPLNAME).nas $(IPLNAME).bin $(IPLNAME).lst

$(OSNAME).img : $(IPLNAME).bin $(OSNAME).sys Makefile
	$(EDIMG) imgin:../z_tools/fdimg0at.tek   \
		wbinimg src:$(IPLNAME).bin len:512 from:0 to:0 \
		copy from:$(OSNAME).sys to:@: \
		imgout:$(OSNAME).img

# 生成规则
%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

# todo 为何是 $* 而不是 $

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

asm :
	$(MAKE) $(IPLNAME).bin

img :
	$(MAKE) $(OSNAME).img

run :
	$(MAKE) img
	$(COPY) $(OSNAME).img ..\z_tools\qemu\fdimage0.bin
	$(MAKE)	-C ../z_tools/qemu

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys

#仅仅保留源文件
src_only :
	$(MAKE) clean
	-$(DEL) $(OSNAME).img

run_and_clean:
	$(MAKE) run
	$(MAKE) src_only

clean_and_run:
	$(MAKE) src_only
	$(MAKE) naskfunc.obj
