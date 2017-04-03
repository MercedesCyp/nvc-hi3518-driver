SHELL := /bin/bash
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
	PARAM_FILE := $(PWD)/Makefile.param
        include $(PARAM_FILE)
endif


DRIVE_NAME = NV_Driver

DEVICE_RELYON = Drvmain.o
DEVICE_RELYON += LinuxAPI.o

ifeq ($(chip),3518)
DEVICE_RELYON += ./HAL/HI3518_PIN.o
DEVICE_RELYON += ./HAL/HI3518_PWM.o
DEVICE_RELYON += ./HAL/HI3518_ADC.o
DEVICE_RELYON += ./HAL/HI3518_TIMER.o
DEVICE_RELYON += ./HAL/PeriodEvent.o
endif
DEVICE_RELYON += ./HAL/HAL.o

DEVICE_RELYON += ./Tool/String.o
DEVICE_RELYON += ./Tool/Mqueue.o
DEVICE_RELYON += ./Tool/MsgCenter.o
DEVICE_RELYON += ./Tool/MsgConf.o
DEVICE_RELYON += ./Tool/NTC.o

DEVICE_RELYON += ./Pro/ProConfig.o
DEVICE_RELYON += ./Pro/ProAbility.o
DEVICE_RELYON += ./Pro/3518C_D01.o
DEVICE_RELYON += ./Pro/3518C_D03.o
DEVICE_RELYON += ./Pro/3518C_D04.o
DEVICE_RELYON += ./Pro/3518C_D11.o
DEVICE_RELYON += ./Pro/3518E_F05.o
DEVICE_RELYON += ./Pro/3518E_F08.o
DEVICE_RELYON += ./Pro/3518E_F09.o
DEVICE_RELYON += ./Pro/3518E_0301.o

# DEVICE_RELYON += ./CTA/CTA.o
DEVICE_RELYON += ./CTA/AudioPlug.o
DEVICE_RELYON += ./CTA/StateLi.o
DEVICE_RELYON += ./CTA/NightLi.o
DEVICE_RELYON += ./CTA/InfraredLi.o
DEVICE_RELYON += ./CTA/LDR.o
DEVICE_RELYON += ./CTA/IfrFilter.o
DEVICE_RELYON += ./CTA/Button.o
DEVICE_RELYON += ./CTA/DoubLens.o
DEVICE_RELYON += ./CTA/TempMonitor.o
DEVICE_RELYON += ./CTA/CoolFan.o
DEVICE_RELYON += ./CTA/PanTilt.o









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
#KERNEL_SRC = $(shell echo $$NVC_HISDK3518/Hi3518_SDK_V1.0.8.1/osdrv/kernel/linux-3.0.y)

obj-m := $(DRIVE_NAME).o
$(DRIVE_NAME)-objs := $(DEVICE_RELYON)

ifeq ($(chip),3507)
EXTRA_CFLAGS += -D PLATFORM_3507
endif

ifeq ($(chip),3518)
EXTRA_CFLAGS += -D PLATFORM_3518
endif


all:
	@make -C $(KERNEL_SRC) M=${PWD} modules
	-@cp $(DRIVE_NAME).ko $(DIR_TarDrive)
#   -@rm -rf ${DEL_FILE}
#	@make -C Application/
#	@rm -rf Application/*.o
	
clean:
	@rm -rf ${DEL_FILE}

clr:
	@rm -rf ${DEL_FILE}

#copy the file to tar dirctor   
install:
ifeq ($(NVC_BASE_ROOT_DIR_EXIST),exist)
	mkdir -p ${NVC_BASE_DIR}ko/NV_Driver/
	cp $(DRIVE_NAME).ko ${NVC_BASE_DIR}ko/NV_Driver/ -f
else
	echo "nvc base dir not exist >&2"
endif

