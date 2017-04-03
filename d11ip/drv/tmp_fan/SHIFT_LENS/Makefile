#make -C /xulei/ar9331/release/build/../linux/kernels/mips-linux-2.6.31 SUBDIRS=/xulei/ar9331/release/drivers/wlan/os/linux modules

ifeq ($(PARAM_FILE), )
        PARAM_FILE:=../../../Makefile.param
        include $(PARAM_FILE)
endif



DRIVE_NAME = SHITF_LENS
DEVICE_RELYON = DIV_main.o REGShiftLens.o

#SAVE_PATH   := ../../../build/$(DRIVE_NAME)/
SAVE_PATH   := ../../../build/tmp_fan/

DEL_FILE = *.ko *.o *.symvers *.order *.mod.* 
DEL_FILE += \.*\.cmd 
DEL_FILE += \.*\.mod.o.cmd
DEL_FILE += \.*\.ko.cmd
DEL_FILE += .tmp_versions

DIR_TarDrive = /mnt/nfs

PWD = ${shell pwd}

obj-m := $(DRIVE_NAME).o
$(DRIVE_NAME)-objs := $(DEVICE_RELYON)

all:
	@make -C $(LINUX_ROOT) M=${PWD} modules
	mkdir -p $(SAVE_PATH)
	cp $(DRIVE_NAME).ko  $(SAVE_PATH)
#	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)
	@rm -rf ${DEL_FILE}
	
clean:
	@rm -rf ${DEL_FILE}
	
#copy the file to tar dirctor	
CTT:
	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)

