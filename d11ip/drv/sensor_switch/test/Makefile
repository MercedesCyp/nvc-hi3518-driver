
DRIVER_NAME := test_ss
SAVE_PATH   := ../../../build/$(DRIVER_NAME)/

ifeq ($(PARAM_FILE), )
                PARAM_FILE:= ../../../Makefile.param
                include $(PARAM_FILE)
endif
			
OBJS := $(DRIVER_NAME).c 
FLAGS := -Wall
 
test_$(DRIVER_NAME): $(OBJS)
	$(CC) $^ $(FLAGS) -o $@
	@mkdir -p $(SAVE_PATH)
	cp test_$(DRIVER_NAME) $(SAVE_PATH)
clean:
	rm -rf *.o test_$(DRIVER_NAME)


