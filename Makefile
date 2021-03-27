TOOLPATH	= ../z_tools/
MAKE		= $(TOOLPATH)make.exe -r
NASK		= $(TOOLPATH)nask.exe
EDIMG		= $(TOOLPATH)edimg.exe
IMGTOL		= $(TOOLPATH)imgtol.exe
COPY		= copy
DEL			= del
OSNAME		= haribote
IPLNAME		= ipl10

$(IPLNAME).bin : $(IPLNAME).nas Makefile
	$(NASK) $(IPLNAME).nas $(IPLNAME).bin $(IPLNAME).lst

$(OSNAME).sys : $(OSNAME).nas Makefile
	$(NASK) $(OSNAME).nas $(OSNAME).sys $(OSNAME).lst

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
	-$(DEL) $(IPLNAME).bin
	-$(DEL) $(IPLNAME).lst
	-$(DEL) $(OSNAME).sys
	-$(DEL) $(OSNAME).lst

#仅仅保留源文件
src_only :
	$(MAKE) clean
	-$(DEL) $(OSNAME).img