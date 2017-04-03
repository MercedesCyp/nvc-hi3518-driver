


DRIVER_NAME := nl
SAVE_PATH   := ../../../build/$(DRIVER_NAME)/

ifeq ($(PARAM_FILE), )
                PARAM_FILE:= ../../../Makefile.param
                include $(PARAM_FILE)
endif
			
OBJS := test_$(DRIVER_NAME).c 
FLAGS := -Wall
 
test_$(DRIVER_NAME): $(OBJS)
	$(CC) $^ $(FLAGS) -o $@
	@mkdir -p $(SAVE_PATH)
	cp test_nl $(SAVE_PATH)/test_nl_loop
clean:
	rm -rf *.o test_$(DRIVER_NAME)


