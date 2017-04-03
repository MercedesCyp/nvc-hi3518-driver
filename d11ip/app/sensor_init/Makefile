
#include /hiwork/Hi3518_SDK_V1.0.8.1/mpp2/extdrv/hi_i2c
#all:
#	arm-hisiv100nptl-linux-gcc ov9712_sensor_ctl.c -o ov9712_init.o
#	cp

export SDK_PATH?=$(shell echo $$NVC_HISDK3518)/Hi3518_SDK_V1.0.8.1

DRIVER_NAME := sensor_init
SAVE_PATH   := ../../build/$(DRIVER_NAME)/

#ifeq ($(PARAM_FILE), )
#                PARAM_FILE:= ../../../Makefile.param
#                include $(PARAM_FILE)
#endif
			
OBJS := $(DRIVER_NAME).c 
FLAGS := -Wall
 
$(DRIVER_NAME): $(OBJS)
	arm-hisiv100nptl-linux-gcc $^ $(FLAGS) -I$(SDK_PATH)/mpp2/extdrv/hi_i2c/    -o $@
	mkdir -p $(SAVE_PATH)
	cp $(DRIVER_NAME) $(SAVE_PATH)
clean:
	rm -rf *.o $(DRIVER_NAME)


