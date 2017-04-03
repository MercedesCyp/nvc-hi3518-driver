CROSS_COMPILE = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-

CC = $(CROSS_COMPILE)gcc

APPLICATION_RELYON = main.o
TAR_FILE = TEST
TAR_DIR = /mnt/nfs

DELFILE = TEST
DELFILE += *.o

all: $(TAR_FILE) mov clean


$(TAR_FILE): $(APPLICATION_RELYON)
	$(CC) -o $(TAR_FILE) $(APPLICATION_RELYON)
mov:
	@cp $(TAR_FILE) $(TAR_DIR)

clean:
	@rm $(DELFILE)