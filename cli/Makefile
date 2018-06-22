SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I../ -I../cloud_connector/ -I../cloud_structures/ -I../common/ 
OBJS      = $(subst .c,,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif
all: $(OBJS)

%: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) ../cloud_connector/cloud_connector.o ../cloud_structures/cloud_structures.o ../common/common.o $< -o $@ -ldl

clean: 
	rm -f $(OBJS)
