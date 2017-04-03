SHELL = /bin/bash


#CSDK=PC
CSDK=HISI
#HISISDK=3507
HISISDK=3518

# select compiler Tools
ifeq ($(CSDK),PC)

endif

ifeq ($(CSDK),HISI)
ifeq ($(HISISDK),3507)
	COMPTOOLS=arm-hismall-linux-
endif
ifeq ($(HISISDK),3518)
	COMPTOOLS=arm-hisiv100nptl-linux-
endif
endif
#compiler prelude
CC=$(COMPTOOLS)gcc


APPNAME=driver_t

DEPENDS += T_MAIN.c
DEPENDS += T_SHOW.c
DEPENDS += resolve.c
DEPENDS += MsgCent.c
#DEPENDS += ReportCent.c

all:
	@echo -e "\e[1;32m"
	-@$(CC) $(DEPENDS) -o $(APPNAME) -lpthread
	@echo -e "\e[0m "
	@test ! -d ./T && mkdir ./T || true
	@cp $(APPNAME) README ./T
	@cp -rf ./T /mnt/nfs
	@rm -rf ./T


