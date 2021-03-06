#所涉及到的目标(obj)文件集合
OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj signalbuffer.obj kbc.obj mousedecoder.obj global.obj memorymanager.obj \
				sheet.obj timercb.obj task.obj textbox.obj string.obj file.obj

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

$(OSNAME).img : $(IPLNAME).bin $(OSNAME).sys hlt.hrb a.hrb crack.hrb Makefile
	$(EDIMG) imgin:../z_tools/fdimg0at.tek   \
		wbinimg src:$(IPLNAME).bin len:512 from:0 to:0 \
		copy from:$(OSNAME).sys to:@: \
		copy from:ipl10.nas to:@: \
        copy from:make.bat to:@: \
        copy from:hlt.hrb to:@: \
        copy from:a.hrb to:@: \
        copy from:crack.hrb to:@: \
		imgout:$(OSNAME).img

a.bim : a.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj a_nask.obj

a.hrb : a.bim Makefile
	$(BIM2HRB) a.bim a.hrb 0

crack.bim : crack.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:crack.bim map:crack.map crack.obj

crack.hrb : crack.bim Makefile
	$(BIM2HRB) crack.bim crack.hrb 0

hlt.hrb : hlt.nas Makefile
	$(NASK) hlt.nas hlt.hrb hlt.lst
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
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb
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
