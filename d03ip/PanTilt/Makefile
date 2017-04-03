#make -C /xulei/ar9331/release/build/../linux/kernels/mips-linux-2.6.31 SUBDIRS=/xulei/ar9331/release/drivers/wlan/os/linux modules

ifeq ($(PARAM_FILE), )
        #PARAM_FILE:=../../../../Hi3518_SDK_V1.0.8.1/mpp/Makefile.param
		PARAM_FILE:=/Hi3518_SDK_V1.0.8.1/mpp/Makefile.param
        include $(PARAM_FILE)
endif

DRIVE_NAME = DIV_PanTilt
DEVICE_RELYON = DIV_main.o PanTilt.o HAL_Timer.o PanTilt_FileOpts.o

DEL_FILE = *.ko *.o *.symvers *.order *.mod.* 
DEL_FILE += \.*\.cmd 
DEL_FILE += \.*\.mod.o.cmd
DEL_FILE += \.*\.ko.cmd
DEL_FILE += .tmp_versions

DIR_TarDrive = /mnt/nfs

PWD = ${shell pwd}
KERNEL_SRC = /Hi3518_SDK_V1.0.8.1/osdrv/kernel/linux-3.0.y

obj-m := $(DRIVE_NAME).o
$(DRIVE_NAME)-objs := $(DEVICE_RELYON)

all:
#	make -C $(LINUX_ROOT) M=${PWD} modules
	@make -C $(KERNEL_SRC) M=${PWD} modules
	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)
	@rm -rf ${DEL_FILE}
	
clean:
	@rm -rf ${DEL_FILE}
	
#copy the file to tar dirctor	
CTT:
	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)
