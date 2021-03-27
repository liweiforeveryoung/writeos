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
RULEFILE = $(TOOLPATH)haribote/haribote.rul

bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

bootpack.bim : bootpack.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj
# 3MB+64KB=3136KB
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

# 连接 asmhead.bin 和 bootpack.hrb
$(OSNAME).sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb haribote.sys

$(IPLNAME).bin : $(IPLNAME).nas Makefile
	$(NASK) $(IPLNAME).nas $(IPLNAME).bin $(IPLNAME).lst

$(OSNAME).img : $(IPLNAME).bin $(OSNAME).sys Makefile
	$(EDIMG) imgin:../z_tools/fdimg0at.tek   \
		wbinimg src:$(IPLNAME).bin len:512 from:0 to:0 \
		copy from:$(OSNAME).sys to:@: \
		imgout:$(OSNAME).img

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
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys

#仅仅保留源文件
src_only :
	$(MAKE) clean
	-$(DEL) $(OSNAME).img