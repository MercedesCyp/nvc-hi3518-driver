SHELL := /bin/bash
#make -C /xulei/ar9331/release/build/../linux/kernels/mips-linux-2.6.31 SUBDIRS=/xulei/ar9331/release/drivers/wlan/os/linux modules
ifndef CHECK_DIR_EXIST
define CHECK_DIR_EXIST
$(if $(strip $(1)),$(shell if [ -d ${1} ]; then echo "exist"; else echo "noexist"; fi),noexist)
endef
endif
export NV_CROSS_COMPILE?=arm-hisiv100nptl-linux
NV_COMPILE_ARCH_NAME:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE),linux)
NVC_BASE_ROOT_DIR:=$(shell echo `[[ -n $$NVC_BASE ]] && echo -n $$NVC_BASE && echo "/" || echo ""` | tr -s '[/]')
NVC_BASE_ROOT_DIR_EXIST := $(call CHECK_DIR_EXIST, $(NVC_BASE_ROOT_DIR))
NVC_BASE_DIR:=$(if $(strip $(NVC_BASE_ROOT_DIR)),$(NVC_BASE_ROOT_DIR)$(NV_COMPILE_ARCH_NAME)/,)

ifeq ($(PARAM_FILE), )
        #PARAM_FILE:=../../../../Hi3518_SDK_V1.0.8.1/mpp/Makefile.param
		PARAM_FILE := ./Makefile.param
        include $(PARAM_FILE)
endif

DRIVE_NAME = NV_Driver
DEVICE_RELYON = DIV_main.o
DEVICE_RELYON += FunMaster.o
DEVICE_RELYON += Mqueue.o
DEVICE_RELYON += MsgCenter.o
DEVICE_RELYON += ProInfo.o
DEVICE_RELYON += String.o

DEVICE_RELYON += ./HAL/HAL.o
DEVICE_RELYON += ./HAL/HAL_ADC.o
DEVICE_RELYON += ./HAL/HAL_PWM.o
DEVICE_RELYON += ./HAL/HAL_Timer.o
DEVICE_RELYON += ./HAL/PeriodEvent.o
DEVICE_RELYON += ./HAL/SHT20.o

DEVICE_RELYON += ./AudioPlug/AudioPlug.o
DEVICE_RELYON += ./ButtonMonitor/Button.o
DEVICE_RELYON += ./CoolFan/CoolFan.o
DEVICE_RELYON += ./DNMonitor/DNMonitor.o
DEVICE_RELYON += ./DoubleLens/DoubleLens.o
DEVICE_RELYON += ./Humidity/Humidity.o
DEVICE_RELYON += ./IfrFilter/IfrFilter.o
DEVICE_RELYON += ./IfrLight/IfrLight.o
DEVICE_RELYON += ./NightLight/NightLight.o
DEVICE_RELYON += ./PanTilt/PanTilt.o
DEVICE_RELYON += ./StateLED/StateLED.o
DEVICE_RELYON += ./TempMonitor/TempMonitor.o
DEVICE_RELYON += ./TempMonitor/NTC.o

DEL_FILE = *.o *.symvers *.order *.mod.* 
DEL_FILE += \.*\.cmd 
DEL_FILE += \.*\.mod.o.cmd
DEL_FILE += \.*\.ko.cmd
DEL_FILE += .tmp_versions
DEL_FILE += ./*\/\.*\.cmd 
DEL_FILE += ./*\/*\.o

#DEL_FILE += *.ko

DIR_TarDrive = /mnt/nfs

PWD = ${shell pwd}
KERNEL_SRC = $(shell echo $$NVC_HISDK3518/Hi3518_SDK_V1.0.8.1/osdrv/kernel/linux-3.0.y)

obj-m := $(DRIVE_NAME).o
$(DRIVE_NAME)-objs := $(DEVICE_RELYON)

all:
#	make -C $(LINUX_ROOT) M=${PWD} modules
	@make -C $(KERNEL_SRC) M=${PWD} modules
	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)
	@rm -rf ${DEL_FILE}
#	@make -C Application/
#	@rm -rf Application/*.o
	
clean:
	@rm -rf ${DEL_FILE}
	
#copy the file to tar dirctor	
CTT:
	@cp $(DRIVE_NAME).ko $(DIR_TarDrive)

install:
ifeq ($(NVC_BASE_ROOT_DIR_EXIST),exist)
	mkdir -p ${NVC_BASE_DIR}ko/NV_Driver/
	cp $(DRIVE_NAME).ko ${NVC_BASE_DIR}ko/NV_Driver/ -f
else
	echo "nvc base dir not exist >&2"
endif

