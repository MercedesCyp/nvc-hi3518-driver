#
# Copyright (c) 2011 Netviewtech Co.,Ltd.
#
# Description:
#              Linux driver use two or more file, this file try to use. 
#
# File name  : Makefile
# Author     : qiang.feng
# Date       : 2011.12.21
# Version    : 1.00
#

COLOR1=\x1b[0;32m # green


DRIVER_NAME := irc
#SAVE_PATH   := /job/nfs/hi3518c/$(DRIVER_NAME)/
SAVE_PATH   := ../../build/$(DRIVER_NAME)/

obj-m := $(DRIVER_NAME).o
#$(DRIVER_NAME)-y += $(DRIVER_NAME).o


ifeq ($(PARAM_FILE), )
                PARAM_FILE:= ../../Makefile.param
                include $(PARAM_FILE)
endif
			

EXTRA_CFLAGS += -D$(HI_FPGA) 
 
default:	
	@make -C $(LINUX_ROOT) M=$(PWD) modules
	@rm -rf $(SAVE_PATH)$(DRIVER_NAME).ko
	@mkdir -p $(SAVE_PATH)
	@cp ./$(DRIVER_NAME).ko  $(SAVE_PATH)
	@make -C test/
#	@make -C app/mpp
#	@make -C app/mpp2
clean:
	@rm *.o -rf
	@make -C $(LINUX_ROOT) M=$(PWD) clean
	@make -C test clean
#	@make -C app/mpp clean
#	@make -C app/mpp2 clean


