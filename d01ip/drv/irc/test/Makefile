CROSS_COMPILE = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-

CC = $(CROSS_COMPILE)gcc

DRIVER_NAME := irc
#SAVE_PATH   := ../../../build/$(DRIVER_NAME)/
SAVE_PATH   := /mnt/nfs

OBJS := test_$(DRIVER_NAME).c 
FLAGS := -Wall
 
test_$(DRIVER_NAME): $(OBJS)
	$(CC) $^ $(FLAGS) -o $@
	@mkdir -p $(SAVE_PATH)
	cp test_$(DRIVER_NAME) $(SAVE_PATH)
clean:
	rm -rf *.o test_$(DRIVER_NAME)


