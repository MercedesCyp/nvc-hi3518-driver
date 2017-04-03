#CROSS_COMPILE = /opt/hisi-linux/x86-arm/arm-hisiv200-linux/target/bin/arm-hisiv200-linux-
#CROSS_COMPILE = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-

CROSS_COMPILE=arm-hisiv100nptl-linux

CC = $(CROSS_COMPILE)-gcc

#SAVE_PATH   := ../../build/$(DRIVE_NAME)/
SAVE_PATH   := ../../../build/tmp_fan/

TAR_FILE = TEST
TAR_DIR	= /mnt/nfs

all: TEST OTHERS

$(TAR_FILE): main.o NTC.o
	$(CC) -o TEST main.o NTC.o
	
main.o: main.c 
	$(CC) -c main.c
	
NTC.o: NTC.c
	$(CC) -c NTC.c
	

	
OTHERS:
	mkdir -p $(SAVE_PATH)
	cp $(TAR_FILE) $(SAVE_PATH)
	@cp $(TAR_FILE) $(TAR_DIR)
	@rm $(TAR_FILE) *.o
	
clean:
	@rm -f $(TAR_FILE) *.o
