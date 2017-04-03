export NV_COMPILE_SHELL ?= /bin/bash
#export NV_CROSS_COMPILE?=arm-hisiv100nptl-linux
export NV_CROSS_COMPILE?=arm-hisiv300-linux
export NV_COMPILE_ARCH_NAME:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE),linux)
export CC:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-gcc,gcc)
export CXX:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-g++,g++)
export LD:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-ld,ld)
export AR:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-ar,ar)
export TP:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-strip,strip)
export RANLIB:=$(if $(strip $(NV_CROSS_COMPILE)),$(NV_CROSS_COMPILE)-ranlib,ranlib)

ifndef CHECK_DIR_EXIST
define CHECK_DIR_EXIST
$(if $(strip $(1)),$(shell if [ -d ${1} ]; then echo "exist"; else echo "noexist"; fi),noexist)
endef
endif
SHELL := $(NV_COMPILE_SHELL)
NVC_BASE_ROOT_DIR:=$(shell echo `[[ -n $$NVC_BASE ]] && echo -n $$NVC_BASE && echo "/" || echo ""` | tr -s '[/]')
NVC_BASE_ROOT_DIR_EXIST := $(call CHECK_DIR_EXIST, $(NVC_BASE_ROOT_DIR))
ifeq (exist, $(NVC_BASE_ROOT_DIR_EXIST))
MK_NVC_BASE_CROSS_ROOT_DIR:=$(shell mkdir -p $(NVC_BASE_ROOT_DIR)$(NV_COMPILE_ARCH_NAME))
endif
NVC_BASE_DIR:=$(if $(strip $(NVC_BASE_ROOT_DIR)),$(NVC_BASE_ROOT_DIR)$(NV_COMPILE_ARCH_NAME)/,)
NVC_BASE_DIR_WITHOUT_SLASH:=$(shell if [ "$(NVC_BASE_DIR)" == "/" ];then echo "/"; else echo $(NVC_BASE_DIR) | sed 's/\/$$//';fi)
NVC_BASE_DIR_EXIST := $(NVC_BASE_ROOT_DIR_EXIST)
ifeq (exist, $(NVC_BASE_DIR_EXIST))
MK_NVC_BASE_FILE_DIRS:=$(shell mkdir -p $(NVC_BASE_DIR)lib && mkdir -p $(NVC_BASE_DIR)bin && mkdir -p $(NVC_BASE_DIR)unit_tests)
endif

NVC_CAMERA_FS_ROOT_DIR:=$(shell echo `[[ -n $$NVC_CAMERA_FS ]] && echo -n $$NVC_CAMERA_FS && echo "/" || echo ""` | tr -s '[/]')
NVC_CAMERA_FS_ROOT_DIR_EXIST := $(call CHECK_DIR_EXIST, $(NVC_CAMERA_FS_ROOT_DIR))
ifeq (exist, $(NVC_CAMERA_FS_ROOT_DIR_EXIST))
MK_NVC_CAMERA_FS_CROSS_ROOT_DIR:=$(shell mkdir -p $(NVC_CAMERA_FS_ROOT_DIR)$(NV_COMPILE_ARCH_NAME))
endif
NVC_CAMERA_FS_DIR:=$(if $(strip $(NVC_CAMERA_FS_ROOT_DIR)),$(NVC_CAMERA_FS_ROOT_DIR)$(NV_COMPILE_ARCH_NAME)/,)
NVC_CAMERA_FS_DIR_WITHOUT_SLASH:=$(shell if [ "$(NVC_CAMERA_FS_DIR)" == "/" ];then echo "/"; else echo $(NVC_CAMERA_FS_DIR) | sed 's/\/$$//';fi)
NVC_CAMERA_FS_DIR_EXIST := $(NVC_CAMERA_FS_ROOT_DIR_EXIST)
ifeq (exist, $(NVC_CAMERA_FS_DIR_EXIST))
MK_NVC_CAMERA_FS_FILE_DIRS:=$(shell mkdir -p $(NVC_CAMERA_FS_DIR)lib && mkdir -p $(NVC_CAMERA_FS_DIR)bin)
endif

bin_src:=$(wildcard ./test_*.c)
bin_tgt:=$(patsubst ./test_%.c, ./build/test_%, $(bin_src))
lib_tgt:=./build/libnvc-drv.so

all: lib $(bin_tgt)

lib:
	@mkdir -p build
	${CC} -c -fPIC ${CFLAGS} -I. netview_driver_api.c -o ./build/netview_driver_api.o
	${CC} -shared -o ${lib_tgt} ./build/netview_driver_api.o
	$(TP) --remove -section=.note --remove -section=.comment ${lib_tgt}
	rm -f ./build/netview_driver_api.o

./build/test_% : ./test_%.c
	@mkdir -p build
	${CC} ${CFLAGS} -I. $< -o $@ -L./build -lnvc-drv -lpthread
	$(TP) --remove -section=.note --remove -section=.comment $@

clean:
	rm -f ./build/*

install:
ifeq ($(NVC_BASE_DIR_EXIST),exist)
	mkdir -p ${NVC_BASE_DIR}ko/NV_Driver/ ${NVC_BASE_DIR}lib
	cp ${bin_tgt} ${NVC_BASE_DIR}ko/NV_Driver/ -f
	cp ${lib_tgt} ${NVC_BASE_DIR}ko/NV_Driver/ -f
else
	echo "nvc base dir not exist >&2"
endif
ifeq ($(NVC_CAMERA_FS_DIR_EXIST),exist)
	mkdir -p ${NVC_CAMERA_FS_DIR}ko/NV_Driver/ ${NVC_CAMERA_FS_DIR}lib
	cp ${bin_tgt} ${NVC_CAMERA_FS_DIR}ko/NV_Driver/ -f
	cp ${lib_tgt} ${NVC_CAMERA_FS_DIR}lib/ -f
endif

