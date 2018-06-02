SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I../ -I../s3_connector/ -I../obj_structs/ -I../common/ 
OBJS      = $(subst .c,,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif
all: $(OBJS)

%: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) ../s3_connector/s3_connector.o ../obj_structs/obj_structs.o ../common/common.o -ls3 $< -o $@

clean: 
	rm -f $(OBJS)
