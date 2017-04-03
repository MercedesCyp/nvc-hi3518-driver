SHELL := /bin/bash

ifeq ($(PARAM_FILE), )
	PARAM_FILE := $(PWD)/Makefile.param
        include $(PARAM_FILE)
endif

ifndef CHECK_DIR_EXIST
define CHECK_DIR_EXIST
$(if $(strip $(1)),$(shell if [ -d ${1} ]; then echo "exist"; else echo "noexist"; fi),noexist)
endef
endif

ifeq ($(chip),3518)
export NV_CROSS_COMPILE?=arm-hisiv100nptl-linux
endif
ifeq ($(chip),3518ev200)
export NV_CROSS_COMPILE?=arm-hisiv300-linux
endif
ifeq ($(chip),3507)
export NV_CROSS_COMPILE?=arm-hismall-linux
endif

NV_COMPILE_ARCH_NAME:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE),linux)
NVC_BASE_ROOT_DIR:=$(shell echo `[[ -n $$NVC_BASE ]] && echo -n $$NVC_BASE && echo "/" || echo ""` | tr -s '[/]')
NVC_BASE_ROOT_DIR_EXIST := $(call CHECK_DIR_EXIST, $(NVC_BASE_ROOT_DIR))
NVC_BASE_DIR:=$(if $(strip $(NVC_BASE_ROOT_DIR)),$(NVC_BASE_ROOT_DIR)$(NV_COMPILE_ARCH_NAME)/,)

DRIVE_NAME = NV_Driver
DEVICE_RELYON = Drvmain.o
DEVICE_RELYON += LinuxAPI.o

DEVICE_RELYON += ./HAL/PeriodEvent.o
DEVICE_RELYON += ./HAL/HAL.o
DEVICE_RELYON += ./HAL/IIC.o

DEVICE_RELYON += ./Tool/String.o
DEVICE_RELYON += ./Tool/Mqueue.o
DEVICE_RELYON += ./Tool/MsgCenter.o
DEVICE_RELYON += ./Tool/MsgConf.o
DEVICE_RELYON += ./Tool/NTC.o

DEVICE_RELYON += ./Pro/ProConfig.o
DEVICE_RELYON += ./Pro/ProAbility.o

# DEVICE_RELYON += ./CTA/CTA.o
DEVICE_RELYON += ./CTA/AudioPlug.o
DEVICE_RELYON += ./CTA/StateLi.o
DEVICE_RELYON += ./CTA/StaLi_Breath.o
DEVICE_RELYON += ./CTA/NightLi.o
DEVICE_RELYON += ./CTA/InfraredLi.o
DEVICE_RELYON += ./CTA/LDR.o
DEVICE_RELYON += ./CTA/IfrFilter.o
DEVICE_RELYON += ./CTA/Button.o
DEVICE_RELYON += ./CTA/DoubLens.o
DEVICE_RELYON += ./CTA/TempMonitor.o
DEVICE_RELYON += ./CTA/CoolFan.o
DEVICE_RELYON += ./CTA/PanTilt.o
DEVICE_RELYON += ./CTA/DoorBell.o
DEVICE_RELYON += ./CTA/PIR.o
DEVICE_RELYON += ./CTA/DoorLock.o
DEVICE_RELYON += ./CTA/HWAEC.o
DEVICE_RELYON += ./CTA/EAS.o

ifeq ($(chip),3507)

DEVICE_RELYON += ./HAL/HI3507/PIN.o
DEVICE_RELYON += ./HAL/HI3507/TIMER.o
DEVICE_RELYON += ./HAL/HI3507/PCF8563.o
DEVICE_RELYON += ./HAL/HI3507/TLV320AIC23B.o

DEVICE_RELYON += ./CTA/RTC.o

DEVICE_RELYON += ./Pro/3507R_B14.o


endif
ifeq ($(chip),3518)

DEVICE_RELYON += ./HAL/HI3518/PIN.o
DEVICE_RELYON += ./HAL/HI3518/PWM.o
DEVICE_RELYON += ./HAL/HI3518/ADC.o
DEVICE_RELYON += ./HAL/HI3518/TIMER.o

#DEVICE_RELYON += ./Pro/3518C_D01.o
#DEVICE_RELYON += ./Pro/3518C_D03.o
DEVICE_RELYON += ./Pro/3518C_D04.o
DEVICE_RELYON += ./Pro/3518C_D11.o
DEVICE_RELYON += ./Pro/3518E_F05.o
DEVICE_RELYON += ./Pro/3518E_F08.o
DEVICE_RELYON += ./Pro/3518E_F09.o
DEVICE_RELYON += ./Pro/3518E_0301.o
DEVICE_RELYON += ./Pro/3518E_F10.o
DEVICE_RELYON += ./Pro/3518E_0302.o
DEVICE_RELYON += ./Pro/3518E_F16.o
DEVICE_RELYON += ./Pro/3518E_F17.o
DEVICE_RELYON += ./Pro/3518E_G02.o
DEVICE_RELYON += ./Pro/3518E_G03.o
DEVICE_RELYON += ./Pro/3518E_G05.o
DEVICE_RELYON += ./Pro/3518E_G06.o
DEVICE_RELYON += ./Pro/3518E_G07.o
DEVICE_RELYON += ./Pro/3518E_G09.o
DEVICE_RELYON += ./Pro/3518E_G11.o

endif






ifeq ($(chip),3518ev200)

DEVICE_RELYON += ./HAL/HI3518EV200/PIN.o
DEVICE_RELYON += ./HAL/HI3518EV200/PWM.o
DEVICE_RELYON += ./HAL/HI3518EV200/ADC.o
DEVICE_RELYON += ./HAL/HI3518EV200/TIMER.o


DEVICE_RELYON += ./Pro/3518EV200_G16.o
DEVICE_RELYON += ./Pro/3518EV200_G26.o
DEVICE_RELYON += ./Pro/3518EV200_H04.o

endif








DIR_TarDrive = /mnt/nfs

PWD = ${shell pwd}
#KERNEL_SRC = $(shell echo $$NVC_HISDK3518/Hi3518_SDK_V1.0.8.1/osdrv/kernel/linux-3.0.y)

obj-m := $(DRIVE_NAME).o
$(DRIVE_NAME)-objs := $(DEVICE_RELYON)

EXTRA_CFLAGS += -D COMPILER_DRIVER
ifeq ($(chip),3507)
EXTRA_CFLAGS += -D PLATFORM_3507
endif

ifeq ($(chip),3518)
EXTRA_CFLAGS += -D PLATFORM_3518
endif

ifeq ($(chip),3518ev200)
EXTRA_CFLAGS += -D PLATFORM_3518EV200
endif


all:
	@echo -e "\n\n"
	@echo -e "\e[1;32m================ --Compiler & link Start-- ================\e[0m"
	@echo -e "\e[1;33m* - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - *"
	@echo -e "Current compiler envirement: $(chip)"
	@echo -e "ATENTION:\n\tIf you have a different development environment\n\tPlease amend the variables name's \`chip\` in Makefile.param\n\tor you can use the command as below:\n\t\` make chip=3507 \`\n\t\` make chip=3518 \`\n\t\` make chip=3518ev200 \`"
	@echo -e "* - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - *\e[0m"
	@make -C $(KERNEL_SRC) M=${PWD} modules
	-@cp $(DRIVE_NAME).ko $(DIR_TarDrive) -rf
	@echo "$(CROSS)"
#	@make -C Application/
#	@rm -rf Application/*.o
	@echo -e "\e[1;32m================  --Compiler & link End--  ================\e[0m"
	@echo -e "\n\n"
	
clean:
	@for _f in `find . -name "*.o" -o -name "*.cmd" -o -name "*.order" -o -name "*.symvers" -o -name "*.mod.c" `;do rm $$_f;  done
	@rm -rf .tmp_versions
#	@for _f in `find . -name "*.o" `;do echo $$_f;  done
	@echo -e "\e[1;31mTarget has been removing.\e[0m"


clr:clean
#	@for _f in `find . -name "*.o" -o -name "*.cmd" -o -name "*.order" -o -name "*.symvers" -o -name "*.mod.c" `;do rm $$_f;  done
#	@rm -rf .tmp_versions
#	@for _f in `find . -name "*.o" `;do echo $$_f;  done

#copy the file to tar dirctor   
install:
ifeq ($(NVC_BASE_ROOT_DIR_EXIST),exist)
	mkdir -p ${NVC_BASE_DIR}ko/NV_Driver/
	cp $(DRIVE_NAME).ko ${NVC_BASE_DIR}ko/NV_Driver/ -f
else
	echo "nvc base dir not exist >&2"
endif


